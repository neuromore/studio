#version 120
varying   vec4 colorV;

void main(void)
 {
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;		
    gl_TexCoord[0] = gl_MultiTexCoord0;
    colorV = gl_Color;
 }

 void HeatmapWidget::InitShader()
{
	const bool compileShaderFromFile = false;

	if (compileShaderFromFile)
	{
		// Compile vertex shader
		if (!mProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Assets/Shaders/HeatmapVertexShader.glsl"))
			close();

		// Compile fragment shader
		if (!mProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Assets/Shaders/HeatmapFragmentShader.glsl"))
			close();
	}
	else
	{
	    // source for vertexshader
	    String vertexSource        = "#version 120\n"
			                         "varying   vec4 colorV;\n"
									 "void main(void) {\n"
									 "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
									 "gl_TexCoord[0] = gl_MultiTexCoord0;\n"
									 "colorV = gl_Color;}";

		// source for fragmentshader
		String fragmentSource      = "#version 120\n" 
			                         "varying vec4 colorV;\n"
									 "void main(void) {\n"
									 "vec2 pos = mod(gl_FragCoord.xy, vec2(185.0)) - vec2(60.0);\n" 
									 "float dist_squared = dot(pos, pos);\n" 
									 "if (dist_squared < 50.0) {\n" 
									 "gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);}\n" 
									 "else if (50.0 < dist_squared && dist_squared < 900.0) {\n" 
									 "float frac = (dist_squared * 0.0011);\n" 
									 "gl_FragColor = vec4(1.0, frac, frac, 1.0) * colorV;}\n" 
									 "else {\n" 
									 "gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);}}";

		if (!mProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexSource))
			close();

		if (!mProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentSource))
			close();
	}

    // Link shader pipeline
    if (!mProgram.link())
        close();

    // Bind shader pipeline for use
    if (!mProgram.bind())
        close();
}