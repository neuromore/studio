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

#ifndef __NEUROMORE_BRAINALIVENODE_H
#define __NEUROMORE_BRAINALIVENODE_H

#include "../../Config.h"

#ifdef INCLUDE_DEVICE_BRAINALIVE

#include "../../Graph/DeviceInputNode.h"
#include "BrainAliveDevices.h"
#include "../../Graph/DeviceInputNode.h"
#include "BrainAliveConstant.h"
//#include "../../../Studio/Devices/BrainAlive/BrainAliveBluetooth.h"
//#include "qt/qwid"

class BrainAliveNodeBase : public DeviceInputNode {
public:
  ~BrainAliveNodeBase() {}
  BrainAliveNodeBase(Graph *parentGraph, uint32 deviceType)
      : DeviceInputNode(parentGraph, deviceType) {
    mBoardID = BrainAliveDevice::TYPE_ID;
  }
  
  void Init() override;

  const BrainAliveInputParams& GetParams() const { return mParams; }
  int GetBoardID() const { return mBoardID; }

  void OnAttributesChanged() override;

  BrainAliveDevice *GetCurrentDevice() {
    return dynamic_cast<BrainAliveDevice *>(mCurrentDevice);
  }

  Device *FindDevice();

  void ReInit(const Core::Time &elapsed, const Core::Time &delta) override;

protected:
  void SynchronizeParams();
  void CreateNewDevice();

  int mBoardID;
  BrainAliveInputParams mParams;
};

class ENGINE_API BrainAliveNode : public BrainAliveNodeBase {
public:
  enum { TYPE_ID = 0xD00000 | BrainAliveDevice::TYPE_ID };
  static const char *Uuid() { return "0000fe40-cc7a-482a-984a-7f2ed5b3e58f"; }

  ~BrainAliveNode() {}

  BrainAliveNode(Graph *parentGraph)
      : BrainAliveNodeBase(parentGraph, BrainAliveDevice::TYPE_ID) {}
  Core::Color GetColor() const override { return Core::RGBA(60, 120, 210); }
  uint32 GetType() const override { return BrainAliveNode::TYPE_ID; }
  const char *GetTypeUuid() const override final { return Uuid(); }
  const char *GetReadableType() const override { return "BrainAlive"; }
  const char *GetRuleName() const override final {
    return BrainAliveDevice::GetRuleName();
  }
  GraphObject *Clone(Graph *parentGraph) override {
    BrainAliveNode *clone = new BrainAliveNode(parentGraph);
    return clone;
  }

  private:
 /* QPushButton *Connect;*/
 // BLEInterface *m_bleInterface;
  //QPushButton *Connect;
  //QListWidget *mListWidget;
  //QWidget *mScan_Widget;
  //QPushButton *Ok_Button;
  //QWidget *mwidget_2;
  //QListWidget *mListWidget_2;


};

#endif

#endif
