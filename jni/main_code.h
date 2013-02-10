#ifndef GL_CODE_H_INCLUDED
#define GL_CODE_H_INCLUDED

#ifdef TARGET_ANDROID
//#include "android_api.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef TARGET_ANDROID
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#include "boost/shared_ptr.hpp"
#include "boost/thread/thread.hpp"
#include "boost/asio.hpp"
#include "boost/signal.hpp"
#include "boost/assign.hpp"
#include "boost/variant.hpp"
#include "boost/function.hpp"
#include "boost/bind.hpp"

#include "include/Engine.h"

using namespace SE;

class TAndroidApplication : public TApplication, public TSerializeInterface
{
protected:

	void LoadModels();
	void AddFrameBuffers();
	
public:
	bool Inited;
	bool Loaded;
	
	std::vector<boost::variant<boost::function<cardinal()>, boost::function<bool()>, boost::function<void()> > > LoadingQueue;
	
	TLiteModel* LiteModel;

	float WaterTimer;
	float SnowTimer;
	float CloudTimer;
	float SkyTexShift;
	bool WaveDir;
	
	std::vector<TLiteModel> IceModel;
	
	std::shared_ptr<TSimpleLandClass> SimpleLand;
	
	std::shared_ptr<TSimpleLandClass> SimpleLandInv;

	TAndroidApplication();
	
     virtual void InnerInit();
    
     virtual void InnerDeinit();
	
	 virtual void InnerDraw();
	
	 virtual void InnerUpdate(cardinal dt);
	 
	 virtual void InnerOnMove(vec2 shift);

	 virtual void OnMouseDown(TMouseState& mouseState);

	bool IsInited() { return Inited; }
	
	void DrawSceneWithoutWater(bool inv);
	void DrawSnow();
	void DrawAllScene(bool toScreen);


	virtual void Serialize(boost::property_tree::ptree& propertyTree);

	//void DrawSceneWithoutWaterFrame();
	
	
};


//static void checkGlError(const std::string& op);


#endif
