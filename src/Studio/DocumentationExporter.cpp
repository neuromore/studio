/****************************************************************************
**
** Copyright 2019 neuromore co
** Contact: https://neuromore.com/contact
**
** Commercial License Usage
** Licensees holding valid commercial neuromore licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and neuromore. For licensing terms
** and conditions see https://neuromore.com/licensing. For further
** information use the contact form at https://neuromore.com/contact.
**
** neuromore Public License Usage
** Alternatively, this file may be used under the terms of the neuromore
** Public License version 1 as published by neuromore co with exceptions as 
** appearing in the file neuromore-class-exception.md included in the 
** packaging of this file. Please review the following information to 
** ensure the neuromore Public License requirements will be met: 
** https://neuromore.com/npl
**
****************************************************************************/


// include require files
#include "AppManager.h"
#include <Core/StandardHeaders.h>
#include "DocumentationExporter.h"

#include <QFileDialog>
#include <QMessageBox>
#include <Core/Array.h>

using namespace Core;

// TODO move marktdown doc exporter into own file
void DocumentationExporter::ExportNodeMarkdown()
{
	// make sure we have an active classifier
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	if (classifier == NULL)
	{
		QMessageBox::critical(GetQtBaseManager()->GetMainWindow(), "ERROR", "Load the classifier containing all nodes you want to export");
		return;
	}
	const uint32 numClassifierNodes = classifier->GetNumNodes();

	// load template
	QFileDialog::Options options;
	QString selectedFilter;
	QString filename = QFileDialog::getOpenFileName(NULL, "Select Templated Input", "", "Markdown (*.md)", &selectedFilter, options);

	// no input file selected
	if (filename.isEmpty() == true)
	{
		QMessageBox::critical(GetQtBaseManager()->GetMainWindow(), "ERROR", "No input file selected.");
		return;
	}

	// collect nodes for each category
	Array<Array<Node*>> nodes;
	nodes.Resize(SPNode::NUM_CATEGORIES);
	const uint32 numObjects = GetGraphObjectFactory()->GetNumRegisteredObjects();
	for (uint32 c = 0; c < SPNode::NUM_CATEGORIES; ++c)
	{
		for (uint32 i = 0; i < numObjects; ++i)
		{
			GraphObject* object = GetGraphObjectFactory()->GetRegisteredObject(i);
			if (object->GetBaseType() == Node::BASE_TYPE)
			{
				Node* node = static_cast<Node*>(object);
				if (node->GetPaletteCategory() == (SPNode::ECategory)c)
					nodes[c].Add(node);
			}
		}
	}

	// search classifier for the node of this type and replace them in the list (so we have initialized nodes)
	for (uint32 c = 0; c < SPNode::NUM_CATEGORIES; ++c)
	{
		const uint32 numNodes = nodes[c].Size();
		for (uint32 n = 0; n < numNodes; ++n)
		{
			Node* node = nodes[c][n];

			// find node of this type in classifier
			Node* activeNode = NULL;
			for (uint32 i = 0; i < numClassifierNodes && activeNode == NULL; ++i)
				if (classifier->GetNode(i)->GetType() == node->GetType())
					activeNode = classifier->GetNode(i);
			// activeNode may be null
			nodes[c][n] = activeNode;
		}
	}

	// buffers
	String input;									// the input string from the file we will apply replacements on
	String tmp;			tmp.Reserve(256);			// temporary string for formating stuff
	String replacement;	replacement.Reserve(4069);	// temporary string for constructing string replacements
	
	// tmps to replace
	const char* tokenEverything			= "[//]: # (NodeDocumentation)";
	const char* tokenNode				= "[//]: # (%s)";
	const char* tokenAttributeList		= "[//]: # (%s:Attributes)";
	const char* tokenAttributeListBegin = "[//]: # (%s:Attributes:Begin)";
	const char* tokenAttributeListEnd	= "[//]: # (%s:Attributes:End)";
	const char* tokenAttribute			= "[//]: # (%s:Attribute:%s)";
	//const char* tokenPorts		= "[//]: # (%s:Ports)";
	//const char* tokenPortsBegin = "[//]: # (%s:Ports:Begin)";
	//const char* tokenPortsEnd	= "[//]: # (%s:Ports:End)";
	//const char* tokenPort		= "[//]: # (%s:Port:%s)";
	
	//
	// 1) read input
	//
	// create the file on disk
	FILE* file;
	file = fopen(filename.toUtf8().data(), "rb\0");

	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	// load the whole file into a string
	char* stringData = new char[fileSize+1];

	if (fread(stringData, fileSize, 1, file) < 0)
	{
		LogError( "Json::ParseFromFile(): Cannot read file '%s'.", filename.toStdString().c_str() );
	}
	stringData[fileSize] = '\0';

	input = stringData;

	fclose(file);
	delete[] stringData;


	//
	// 2) cleanup input (replace everything between begin..end tokens with list tokens)
	//

	// foreach node category..
	for (uint32 c = 0; c < SPNode::NUM_CATEGORIES; ++c)
	{
		// nodes of this category
		const uint32 numNodes = nodes[c].Size();
		for (uint32 n = 0; n < numNodes; ++n)
		{
			Node* node = nodes[c][n];
			if (node == NULL)
				continue;

			// search for the begin token
			tmp.Format(tokenAttributeListBegin, node->GetReadableType());
			if (input.Contains(tmp.AsChar()) == false)
				continue;

			const uint32 startPos = input.Find(tmp.AsChar());

			// search for the end token (including newline!)
			tmp.Format(tokenAttributeListEnd, node->GetReadableType());
			tmp += "\r\n";
			if (input.Contains(tmp.AsChar()) == false)
				continue;

			const uint32 endPos = input.Find(tmp.AsChar());

			// formatting error?
			if (endPos <= startPos)
				continue;

			// remove everything between start and end pos (excludes the End token)
			const uint32 length = endPos - startPos;
			input.Remove(startPos, length);

			// replace the End token with the normal list token so it gets replaced with the new list later
			replacement.Format(tokenAttributeList, node->GetReadableType());
			input.Replace(tmp.AsChar(), replacement.AsChar());
		}
	}

	//
	// 3) level 1: create main structure
	//
	if (input.Contains(tokenEverything) == true)
	{
		replacement.Clear();

		// foreach node category..
		for (uint32 c = 0; c < SPNode::NUM_CATEGORIES; ++c)
		{
			// heading
			replacement += "\r\n";
			replacement += "# ";
			replacement += SPNode::GetCategoryName((SPNode::ECategory)c);
			replacement += " Nodes \r\n";

			// description
			replacement += "\r\n";
			replacement += "This category contains all ";
			replacement += SPNode::GetCategoryDescription((SPNode::ECategory)c);
			replacement += "\r\n";

			// nodes of this category
			const uint32 numNodes = nodes[c].Size();
			for (uint32 n = 0; n < numNodes; ++n)
			{
				Node* node = nodes[c][n];
				if (node == NULL)
					continue;

				// node token
				tmp.Format(tokenNode, node->GetReadableType());
				replacement += tmp.AsChar();
				replacement += "\r\n";
			}
		}

		input.Replace(tokenEverything, replacement.AsChar());
		replacement.Clear();
	}

	//
	// 4) replace level 2 : the node chapters
	//

	// foreach node category..
	for (uint32 c = 0; c < SPNode::NUM_CATEGORIES; ++c)
	{

		// nodes of this category
		const uint32 numNodes = nodes[c].Size();
		for (uint32 n = 0; n < numNodes; ++n)
		{
			Node* node = nodes[c][n];
			if (node == NULL)
				continue;

			// replace all node tokens
			tmp.Format(tokenNode, node->GetReadableType());
			if (input.Contains(tmp.AsChar()) == false)
				continue;

			replacement.Clear();

			// node heading
			replacement += "\r\n";
			replacement += "## ";
			replacement += node->GetReadableType();
			replacement += "\r\n";

			// node icon
			replacement += "\r\n";
			tmp.Format("![%s Node Icon](../neuromoreStudio/Images/Nodes/Icons/%s.png)", node->GetReadableType(), node->GetTypeUuid());
			replacement += tmp.AsChar();
			replacement += "\r\n";

			// node image
			replacement += "\r\n";
			tmp.Format("![%s Node]", node->GetReadableType());
			replacement += tmp.AsChar();
			tmp.Format("(../neuromoreStudio/Images/Nodes/%s Node.png)", node->GetReadableType());
			tmp.Replace(" ", "%20");
			replacement += tmp.AsChar();
			replacement += "\r\n";
			replacement += "\r\n";

			// add attribute list token
			tmp.Format(tokenAttributeList, node->GetReadableType());
			replacement += tmp.AsChar();
			replacement += "\r\n";

			// replace node token
			tmp.Format(tokenNode, node->GetReadableType());
			input.Replace(tmp.AsChar(), replacement.AsChar());
		}
	}

	//
	// 4) replace level 3: the node tables
	// 
	// TODO can be extended with port lists

	// foreach node category..
	for (uint32 c = 0; c < SPNode::NUM_CATEGORIES; ++c)
	{
		// nodes of this category
		const uint32 numNodes = nodes[c].Size();
		for (uint32 n = 0; n < numNodes; ++n)
		{
			Node* node = nodes[c][n];
			if (node == NULL)
				continue;

			// check if input contains tmp for this node
			tmp.Format(tokenAttributeList, node->GetReadableType());
			if (input.Contains(tmp.AsChar()) == true)
			{
				replacement.Clear();

				const uint32 numAttributes = node->GetNumAttributes();

				// count number of visible attributes
				uint32 numVisibleAttributes = 0;
				for (uint32 a = 0; a < numAttributes; ++a)
					if (node->GetAttributeSettings(a)->IsVisible() == true)
						numVisibleAttributes++;

				// no visible attributes
				if (numVisibleAttributes == 0)
				{
					replacement += "This node has no attributes.\r\n";
				}
				else
				{
					// Attribute List Begin token
					tmp.Format(tokenAttributeListBegin, node->GetReadableType());
					replacement += tmp.AsChar();
					replacement += "\r\n";

					// attribute table header
					replacement += "| Attribute | Description |\r\n";
					replacement += "|-----------|-------------|\r\n";


					// replacement all node attribute tmps
					for (uint32 a = 0; a < numAttributes; ++a)
					{
						AttributeSettings* attribute = node->GetAttributeSettings(a);
						/*Attribute* attributeValue = */node->GetAttributeValue(a);
						if (attribute->IsVisible() == false)
							continue;
						else
						{
							// name 
							replacement += "| ";
							replacement += attribute->GetName();

							// description (token)
							replacement += " | ";
							tmp.Format(tokenAttribute, node->GetReadableType(), attribute->GetName());
							replacement += tmp.AsChar();

							replacement += " |\r\n";
						}
					} // end loop attributes
					

					// Attribute List End Token
					tmp.Format(tokenAttributeListEnd, node->GetReadableType());
					replacement += tmp.AsChar();
					replacement += "\r\n";
				}
				

				// replace all occurences of the attributes list token
				tmp.Format(tokenAttributeList, node->GetReadableType());
				input.Replace(tmp.AsChar(), replacement.AsChar());
			}

		} // end loop node
	} // end loop category


	//
	// 5) replace (level 3: the individual node attributes)
	//
	// TODO do the same for port lists, if we want to add it

	// foreach node category..
	for (uint32 c = 0; c < SPNode::NUM_CATEGORIES; ++c)
	{
		// nodes of this category
		const uint32 numNodes = nodes[c].Size();
		for (uint32 n = 0; n < numNodes; ++n)
		{
			Node* node = nodes[c][n];
			if (node == NULL)
				continue;

			// check all node attribute tmps
			const uint32 numAttributes = node->GetNumAttributes();
			for (uint32 a = 0; a < numAttributes; ++a)
			{
				AttributeSettings* attribute = node->GetAttributeSettings(a);
				Attribute* attributeValue = node->GetAttributeValue(a);
				if (attribute->IsVisible() == false)
					continue;
				else
				{
					// the token we want to replace
					tmp.Format(tokenAttribute, node->GetReadableType(), attribute->GetName());

					// replace tmp if present
					if (input.Contains(tmp.AsChar()))
					{
						// create replace string
						replacement.Clear();

						// description string
						replacement += attribute->GetDescription();

						// replacement allowed and default values
						const uint32 numComboValues = attribute->GetNumComboValues();
						if (numComboValues > 0)
						{
							// list all combovalues
							replacement += " Possible values are: ";
							for (uint32 i = 0; i < numComboValues; ++i)
							{
								if (i > 0)
									replacement += ", ";

								replacement += "*";	// in italic and escaped
								replacement += attribute->GetComboValue(i);
								replacement += "*";
							}
							replacement += ". The default value is *";
							replacement += attribute->GetComboValue((uint32)static_cast<AttributeInt32*>(attribute->GetDefaultValue())->GetValue());
							replacement += "*.";
						}
						else if (attributeValue->GetType() == AttributeBool::TYPE_ID) // bool values
						{
							// TODO?
							replacement += " This is a boolean value (on/off).";
						}
						else if (attributeValue->GetType() == AttributeFloat::TYPE_ID) // float number range values
						{
							const double defaultValue = static_cast<AttributeFloat*>(attribute->GetDefaultValue())->GetValue();
							const double minValue = static_cast<AttributeFloat*>(attribute->GetMinValue())->GetValue();
							const double maxValue = static_cast<AttributeFloat*>(attribute->GetMaxValue())->GetValue();

							replacement += " Possible values are in the range ";
							replacement += "[";
							if (minValue < -10e10 )
								tmp = "-&infin;";
							else
								tmp.Format("%.2f", minValue);
							replacement += tmp.AsChar();
							replacement += " .. ";
							if (maxValue > 10e10)
								tmp = "&infin;";
							else
								tmp.Format("%.2f", maxValue);
							replacement += tmp.AsChar();
							replacement += "].";

							replacement += " The default value is ";
							tmp.Format("%.2f.", defaultValue);
							replacement += tmp.AsChar();
						}
						else if (attributeValue->GetType() == AttributeInt32::TYPE_ID) // int number range values
						{
							const int32 defaultValue = static_cast<AttributeInt32*>(attribute->GetDefaultValue())->GetValue();
							const int32 minValue = static_cast<AttributeInt32*>(attribute->GetMinValue())->GetValue();
							const int32 maxValue = static_cast<AttributeInt32*>(attribute->GetMaxValue())->GetValue();

							replacement += " Possible values are in the range ";
							replacement += "[";

							if (minValue == INT_MIN)
								tmp = "-&infin;";
							else
								tmp.Format("%i", minValue);
							replacement += tmp.AsChar();
							replacement += " .. ";
							if (maxValue == INT_MAX)
								tmp = "&infin;";
							else
								tmp.Format("%i", maxValue);
							replacement += tmp.AsChar();
							replacement += "].";

							replacement += " The default value is ";
							tmp.Format("%i.", defaultValue);
							replacement += tmp.AsChar();
						}

						// now replace the string
						tmp.Format(tokenAttribute, node->GetReadableType(), attribute->GetName());
						input.Replace(tmp.AsChar(), replacement.AsChar());
					}
				}

			}

		} // end loop nodes
	} // end loop categories


	// write out file
	filename = QFileDialog::getSaveFileName(NULL, "Select Output File and Folder", "", "Markdown (*.md)", &selectedFilter, options);

	// no input file selected
	if (filename.isEmpty() == true)
	{
		QMessageBox::critical(GetQtBaseManager()->GetMainWindow(), "ERROR", "No output file selected.");
		return;
	}


	// create the file on disk
	FILE* outFile;
	outFile = fopen(filename.toUtf8().data(), "wt\0");
	fputs(input.AsChar(), outFile);
	fclose(outFile);
}


void DocumentationExporter::ExportNodeTableMarkdown()
{
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	if (classifier == NULL)
	{
		QMessageBox::critical(GetQtBaseManager()->GetMainWindow(), "ERROR", "Load the classifier containing all nodes you want to export");
		return;
	}
	const uint32 numClassifierNodes = classifier->GetNumNodes();

	// set output file
	QFileDialog::Options options;
	QString selectedFilter;
	QString filename = QFileDialog::getSaveFileName(NULL, "Select File", "", "Markdown Table (*.md)", &selectedFilter, options);

	// no output file selected
	if (filename.isEmpty() == true)
		return;

	// string for writing everything
	String output;
	output.Reserve(4096);
	String tmp;

	// table header

	output += "| Node Name | Type ID | Inputs | Outputs | UUID |\r\n";
	output += "|-----------|:-------:|:------:|:-------:|------|\r\n";

	// output all nodes
	const uint32 numObjects = GetGraphObjectFactory()->GetNumRegisteredObjects();
	for (uint32 c = 0; c < SPNode::NUM_CATEGORIES; ++c)
	{
		// category row (bold)
		output += "| **";
		output += SPNode::GetCategoryName((SPNode::ECategory)c);
		output += "** |  |  |  |  | \r\n";

		for (uint32 i = 0; i < numObjects; ++i)
		{
			GraphObject* object = GetGraphObjectFactory()->GetRegisteredObject(i);
			if (object->GetBaseType() == Node::BASE_TYPE)
			{
				Node* node = static_cast<Node*>(object);
				if (node->GetPaletteCategory() != (SPNode::ECategory)c)
					continue;

				// search classifier for an initialized version of this node so we can access things like uuid and num ports
				Node* activeNode = NULL;
				for (uint32 i = 0; i < numClassifierNodes && activeNode == NULL; ++i)
					if (classifier->GetNode(i)->GetType() == node->GetType())
						activeNode = classifier->GetNode(i);

				// use initialized node instead of the graph object prototype instance
				if (activeNode != NULL)
					node = activeNode;

				// table begin
				output += "| ";
				
				// name
				output += node->GetReadableType();		
				output += " | ";

				// type id
				tmp.Format("%x |", node->GetType());
				output += tmp.AsChar();
				// inputs/outputs
				tmp.Format(" %i |", node->GetNumInputPorts());
				output += tmp.AsChar();
				tmp.Format(" %i | ", node->GetNumOutputPorts());
				output += tmp.AsChar();
				// uuid
				output += node->GetTypeUuid();

				// table and line end
				output += " |\r\n";
			}
		}
	}

	// create the file on disk
	FILE* outFile;
	outFile = fopen(filename.toUtf8().data(), "wt\0");
	fputs(output.AsChar(), outFile);
	fclose(outFile);
}
