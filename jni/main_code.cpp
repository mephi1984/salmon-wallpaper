#include "main_code.h"
#ifdef TARGET_ANDROID
#include "android_api.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "include/Engine.h"

#include "main_code.h"

bool makeShot = true;


extern int TimeOfDayPref;
extern bool SnowPref;

extern float lastOffsetX;

#ifdef TARGET_WIN32
int TimeOfDayPref = 0;
bool SnowPref = false;

#else
extern int TimeOfDayPref;
extern bool SnowPref;

#endif


boost::mutex m2;


class LoadingQueueVisitor : public boost::static_visitor<void>
{
public:
    void operator()(boost::function<cardinal()> f) const
    {
        f();
    }
    
    void operator()(boost::function<bool()> f) const
    {
        f();
    }

	void operator()(boost::function<void()> f) const
    {
        f();
    }
};

TAndroidApplication::TAndroidApplication()
	: TApplication()
	, Inited(false)
	, Loaded(false)
	, LiteModel(NULL)
	, WaterTimer(0)
	, SnowTimer(0)
	, CloudTimer(0)
	, SkyTexShift(0)
	, WaveDir(false)
{
}


void TAndroidApplication::Serialize(boost::property_tree::ptree& propertyTree)
{
	BOOST_FOREACH(boost::property_tree::ptree::value_type &v, propertyTree.get_child("Ices"))
	{

		std::string modelName = v.second.get<std::string>("ModelName");
		float posx = v.second.get<float>("PosX");
		float posz = v.second.get<float>("PosZ");

		float angle = v.second.get<float>("Angle", 0.f);
		float scale = v.second.get<float>("Scale", 1.f);

		mat3 m(vec4(0, cos(angle*pi/360.f), 0, sin(angle*pi/360.f)));

		IceModel.push_back(TLiteModel());
		IceModel[IceModel.size()-1].LoadModel(modelName);
		IceModel[IceModel.size()-1].RotateModel(m);
		IceModel[IceModel.size()-1].ScaleModel(scale);
		IceModel[IceModel.size()-1].MoveModel(vec3(posx, 0, posz));


		IceModel[IceModel.size()-1].UpdateVBO();



	}
}



void TAndroidApplication::LoadModels()
{
	mat3 m(vec4(0,1*sin(pi/8 + pi/2),0,1*cos(pi/8 + pi/2)));
	
	LiteModel = new TLiteModel;
	LiteModel->LoadModel("mountain.lm1");
	LiteModel->ScaleModel(0.5f);
	LiteModel->RotateModel(m);
	LiteModel->MoveModel(vec3(0, 0, 0));
	LiteModel->UpdateVBO();

	std::shared_ptr<boost::property_tree::ptree> p = FileToPropertyTree("ices.xml");

	Serialize(*p);

}

void TAndroidApplication::AddFrameBuffers()
{
	ResourceManager->FrameManager.AddFrameRenderBuffer("WaterFrame", 256, 256);
	ResourceManager->FrameManager.AddFrameRenderBuffer("ScreenshotFrame", 512, 512);

}

void TAndroidApplication::DrawSceneWithoutWater(bool inv)
{


	Renderer->PushProjectionMatrix(1,1);
	Renderer->LoadIdentity();

	if (inv)
	{


		if (TimeOfDayPref == 2)
		{
			glBindTexture(GL_TEXTURE_2D, ResourceManager->TexList["sky_night.png"]);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, ResourceManager->TexList["sky.png"]);
		}
		Renderer->DrawRect(vec2(0.0f, 0.56f), vec2(1.f, 0.f), vec2(0.5f + SkyTexShift, 0.f), vec2(1.f + SkyTexShift, 1.f));

		if (TimeOfDayPref == 1)
		{
			glClear(GL_DEPTH_BUFFER_BIT);
			glBindTexture(GL_TEXTURE_2D, ResourceManager->TexList["final_cloud.png"]);
			Renderer->DrawRect(vec2(0.0f, 0.56f), vec2(4.f, 0.f), vec2(0.0f + SkyTexShift + CloudTimer, 0.f), vec2(1.f + SkyTexShift + CloudTimer, 0.5f));
		}
	}
	else
	{
		if (TimeOfDayPref == 2)
		{
			glBindTexture(GL_TEXTURE_2D, ResourceManager->TexList["sky_night.png"]);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, ResourceManager->TexList["sky.png"]);
		}
		Renderer->DrawRect(vec2(0.0f, 0.54f), vec2(1.f, 1.f), vec2(0.5f + SkyTexShift, 0.f), vec2(1.f + SkyTexShift, 1.f));

		if (TimeOfDayPref == 1)
		{
			glClear(GL_DEPTH_BUFFER_BIT);
			glBindTexture(GL_TEXTURE_2D, ResourceManager->TexList["final_cloud.png"]);
			Renderer->DrawRect(vec2(0.0f, 0.54f), vec2(4.f, 1.f), vec2(0.0f + SkyTexShift + CloudTimer, 0.f), vec2(1.f + SkyTexShift + CloudTimer, 0.5f));
		}
	}



	Renderer->PopProjectionMatrix();

	glClear(GL_DEPTH_BUFFER_BIT);
	Renderer->SetGLCamView();

	if (inv)
	{
		Renderer->TranslateMatrix(vec3(0,0.1f,0));
		Renderer->ScaleMatrix(vec3(1, -1, 1));
	}

	Renderer->PushShader("SimplelightShader");

	vec3 dayColor = vec3(0,0,0);
	vec3 nightColor = vec3(0, 0.1f, 0.2f);

	if (TimeOfDayPref == 0)
	{
		RenderUniform3fv("TimeOfDayColor", dayColor.v);
		RenderUniform1f("TimeOfDayCoef1", 0.75f);
		RenderUniform1f("TimeOfDayCoef2", 0.25f);
	}
	else
		if (TimeOfDayPref == 1)
		{
			RenderUniform3fv("TimeOfDayColor", dayColor.v);
			RenderUniform1f("TimeOfDayCoef1", 0.5f);
			RenderUniform1f("TimeOfDayCoef2", 0.25f);
		}
		else
		{
			RenderUniform3fv("TimeOfDayColor", nightColor.v);
			RenderUniform1f("TimeOfDayCoef1", 0.0f);
			RenderUniform1f("TimeOfDayCoef2", 0.4f);
		}

		
		Renderer->PushPerspectiveProjectionMatrix(pi/6, Renderer->GetMatrixWidth() / Renderer->GetMatrixHeight(), 1.f, 450.f);

		LiteModel->DrawVBO();

		for (int i=0; i<IceModel.size(); i++)
		{
			IceModel[i].DrawVBO();
		}

		Renderer->PopProjectionMatrix();

		Renderer->PopShader();
}

void TAndroidApplication::DrawSnow()
{
	
		Renderer->PushProjectionMatrix(1,1);

		Renderer->LoadIdentity();

		const float multiply_x = 4;
		const float multiply_y = 2;

		Renderer->DrawRect(vec2(0, 0), vec2(1.f, 1.f), vec2(-SkyTexShift, SnowTimer), vec2(multiply_x - SkyTexShift, multiply_y + SnowTimer));
		glClear(GL_DEPTH_BUFFER_BIT);
		Renderer->DrawRect(vec2(0, 0), vec2(1.f, 1.f), vec2(SnowTimer*0.7f+0.3f - SkyTexShift, SnowTimer+0.7f), vec2(multiply_x - SkyTexShift + SnowTimer*0.7f+0.3f, multiply_y + SnowTimer+0.7f));
		glClear(GL_DEPTH_BUFFER_BIT);
		Renderer->DrawRect(vec2(0, 0), vec2(1.f, 1.f), vec2(SnowTimer*0.4f+0.7f - SkyTexShift, SnowTimer+0.3f), vec2(multiply_x - SkyTexShift + SnowTimer*0.4f+0.7f, multiply_y + SnowTimer+0.3f));

		Renderer->PopProjectionMatrix();
}

void TAndroidApplication::DrawAllScene(bool toScreen)
{
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	//glClearColor(1.f, 1.f, 1.f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	Renderer->PushMatrix();
	Renderer->SetGLCamView();

	Renderer->PushShader("ClipShader");


	Renderer->SwitchToFrameBuffer("WaterFrame");
	Renderer->SetGLCamView();
	Renderer->ScaleMatrix(vec3(1, -1, 1));


	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawSceneWithoutWater(true);

	Renderer->ScaleMatrix(vec3(1, -1, 1));

	if (toScreen)
	{
		Renderer->SwitchToScreen();
	}
	else
	{
		Renderer->SwitchToFrameBuffer("ScreenshotFrame");
	}

	Renderer->SetGLCamView();

	Renderer->PushShader("NormShader");
	RenderUniform1f("Time", WaterTimer);
	if (Renderer->GetScreenWidth() < 600)
	{
		RenderUniform1f("WaterScale", 0.7f);
	}
	else
	{
		RenderUniform1f("WaterScale", 1.f);
	}
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ResourceManager->TexList["water_nmap.png"]);
	glActiveTexture(GL_TEXTURE0);
	if (toScreen)
	{
	Renderer->SetFullScreenViewport();
	}
	else
	{
		Renderer->SetFrameViewport("ScreenshotFrame");
	}
	Renderer->DrawFramePartScreen("WaterFrame", vec2(0, 0), vec2(1.f, 0.54f));
	Renderer->PopShader();

	glClear(GL_DEPTH_BUFFER_BIT);

	Renderer->SetGLCamView();


	DrawSceneWithoutWater(false);

	glClear(GL_DEPTH_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, ResourceManager->TexList["snow.png"]);

	if (SnowPref)
	{
		DrawSnow();
	}

	Renderer->PopShader();

	Renderer->PopMatrix();
}

void TAndroidApplication::InnerInit()
{
	m2.lock();

	CheckGlError();

	glEnable(GL_BLEND);

	*Console<<"Inner init go!\n";

#ifdef TARGET_ANDROID
	ST::PathToResources = "";
#endif
#ifdef TARGET_WIN32
	ST::PathToResources = "../../assets/";
#endif
	ResourceManager->ShaderManager.AddShader("DefaultShader", "gui_transparent.vertex", "gui_transparent.fragment");
	Renderer->PushShader("DefaultShader");
	
	LoadingQueue.push_back(boost::function<bool()>(boost::bind(&TShaderManager::AddShader, &(ResourceManager->ShaderManager), "ClipShader", "gui_transparent_clip.vertex", "gui_transparent_clip.fragment")));
	LoadingQueue.push_back(boost::function<bool()>(boost::bind(&TShaderManager::AddShader, &(ResourceManager->ShaderManager), "NormShader", "test_norm.vertex", "test_norm.fragment")));
	LoadingQueue.push_back(boost::function<bool()>(boost::bind(&TShaderManager::AddShader, &(ResourceManager->ShaderManager), "ParallaxShader", "test_parallax.vertex", "test_parallax.fragment")));
	LoadingQueue.push_back(boost::function<bool()>(boost::bind(&TShaderManager::AddShader, &(ResourceManager->ShaderManager), "SimplelightShader", "test_simplelight.vertex", "test_simplelight.fragment")));
	
	if (IsFileExistsInUserData("file.bmp"))
	{
		ResourceManager->TexList.AddTextureFromUserdata("file.bmp", "ScreenshotTexture");
	}
	else
	{
		ResourceManager->TexList.AddTexture("file.bmp", "ScreenshotTexture");
	}

	LoadingQueue.push_back(boost::function<cardinal()>(boost::bind(&TTextureListClass::AddTexture, &(ResourceManager->TexList), "console_bkg.bmp", "")));
	LoadingQueue.push_back(boost::function<cardinal()>(boost::bind(&TTextureListClass::AddTexture, &(ResourceManager->TexList), "water_nmap.png", "")));
	LoadingQueue.push_back(boost::function<cardinal()>(boost::bind(&TTextureListClass::AddTexture, &(ResourceManager->TexList), "sky.png", "")));
	LoadingQueue.push_back(boost::function<cardinal()>(boost::bind(&TTextureListClass::AddTexture, &(ResourceManager->TexList), "sky_night.png", "")));
	LoadingQueue.push_back(boost::function<cardinal()>(boost::bind(&TTextureListClass::AddTexture, &(ResourceManager->TexList), "snow.png", "")));
	LoadingQueue.push_back(boost::function<cardinal()>(boost::bind(&TTextureListClass::AddTexture, &(ResourceManager->TexList), "final_cloud.png", "")));

	
	LoadingQueue.push_back(boost::function<bool()>(boost::bind(&TModelManager::AddLiteModel, &(ResourceManager->ModelManager), "mountain.lm1", "")));
	LoadingQueue.push_back(boost::function<bool()>(boost::bind(&TModelManager::AddLiteModel, &(ResourceManager->ModelManager), "ice1.lm1", "")));
	LoadingQueue.push_back(boost::function<bool()>(boost::bind(&TModelManager::AddLiteModel, &(ResourceManager->ModelManager), "ice2.lm1", "")));
	LoadingQueue.push_back(boost::function<bool()>(boost::bind(&TModelManager::AddLiteModel, &(ResourceManager->ModelManager), "ice3.lm1", "")));
	LoadingQueue.push_back(boost::function<bool()>(boost::bind(&TModelManager::AddLiteModel, &(ResourceManager->ModelManager), "ice4.lm1", "")));
	LoadingQueue.push_back(boost::function<bool()>(boost::bind(&TModelManager::AddLiteModel, &(ResourceManager->ModelManager), "ice5.lm1", "")));
	LoadingQueue.push_back(boost::function<bool()>(boost::bind(&TModelManager::AddLiteModel, &(ResourceManager->ModelManager), "ice6.lm1", "")));
	
	LoadingQueue.push_back(boost::function<void()>(boost::bind(&TAndroidApplication::LoadModels, this)));
	LoadingQueue.push_back(boost::function<void()>(boost::bind(&TAndroidApplication::AddFrameBuffers, this)));



	//LoadModels();
	
	CheckGlError();


	boost::get<TPanoramicCamera>(Renderer->Camera).MovePhi(pi/32);

	
	if (Renderer->GetScreenWidth() > Renderer->GetScreenHeight())
	{
		boost::get<TPanoramicCamera>(Renderer->Camera).MoveDist(30.f);
	}

	else
	{
		boost::get<TPanoramicCamera>(Renderer->Camera).MoveDist(45.f);
	}

	boost::get<TPanoramicCamera>(Renderer->Camera).CalcCamVec();

	CheckGlError();

	ResourceManager->LightManager.SetLightOn();

	ResourceManager->LightManager.SetLightDirection(vec3(1, -1, 0));

	//AddFrameBuffers();

	//Renderer->SetPerspectiveFullScreenViewport();

	if (Renderer->GetScreenWidth() < Renderer->GetScreenHeight())
	{
		Renderer->SetMatrixWidth(480);
		Renderer->SetMatrixHeight(800);
	}
	
	Renderer->PushPerspectiveProjectionMatrix(pi/6, Renderer->GetMatrixWidth() / Renderer->GetMatrixHeight(), 1.f, 400.f);

	*Console<<"Inner init end!\n";

	m2.unlock();
}



void TAndroidApplication::InnerDeinit()
{
	m2.lock();
	Inited = false;
	Loaded = false;

	makeShot = true;
	LoadingQueue.clear();
	
	if (LiteModel != NULL)
	{
		LiteModel->FreeModel();
		delete LiteModel;
		LiteModel = NULL;
	}

	IceModel.clear();

	SimpleLand = std::shared_ptr<TSimpleLandClass>();
	SimpleLandInv = std::shared_ptr<TSimpleLandClass>();
	m2.unlock();
}


void TAndroidApplication::InnerDraw()
{
	m2.lock();
	if (!Loaded)
	{
		glClearColor(0.f, 0.f, 0.f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D, ResourceManager->TexList["ScreenshotTexture"]);
		Renderer->PushProjectionMatrix(1,1);
		Renderer->LoadIdentity();
		Renderer->DrawRect(vec2(0, 0), vec2(1.f, 1.f));
		Renderer->PopProjectionMatrix();
		
	}
	else
	{


	DrawAllScene(true);
	
	if (makeShot)
	{
		makeShot = false;

		Renderer->SwitchToFrameBuffer("ScreenshotFrame");

		DrawAllScene(false);

		ResourceManager->TexList.SaveTexDataToPlainBmpToUserData("file.bmp", ResourceManager->FrameManager.ReadFromBufferToTexData("ScreenshotFrame"));

		Renderer->SwitchToScreen();
		
	}
	}
	m2.unlock();

}


void TAndroidApplication::InnerUpdate(cardinal dt)
{

	if (!Loaded)
	{

		boost::apply_visitor( LoadingQueueVisitor(), *LoadingQueue.begin() );
		LoadingQueue.erase(LoadingQueue.begin());

		if (LoadingQueue.size() == 0)
		{
			Loaded = true;
		}
	}
	else
	{


	if (WaveDir)
	{
		WaterTimer += dt/1000.f;
		if (WaterTimer >= 10 * pi)
		{
			WaveDir = false;
			WaterTimer = 10 * pi;
		}
	}
	else
	{
		WaterTimer -= dt/1000.f;
		if (WaterTimer < 0)
		{
			WaveDir = true;
			WaterTimer = 0;
		}
	}

	CloudTimer += dt / 60000.f;

	while (CloudTimer >= 1.f)
	{
		CloudTimer -= 1.f;
	}


	SnowTimer += dt/3000.f;

	while (SnowTimer > 10.f)
	{
		SnowTimer -= 10.f;
	}
	}
	
}

void TAndroidApplication::InnerOnMove(vec2 shift)
{

	shift = vec2(shift.v[0]*Renderer->GetMatrixWidth()/static_cast<float>(Renderer->GetScreenWidth()), shift.v[1]*Renderer->GetMatrixHeight()/static_cast<float>(Renderer->GetScreenHeight()));
	
	boost::get<TPanoramicCamera>(Renderer->Camera).MoveAlpha(-pi*shift.v[0]*0.1f);
}

void TAndroidApplication::OnMouseDown(TMouseState& mouseState)
{
	
}