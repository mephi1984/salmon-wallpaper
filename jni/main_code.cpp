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
    void operator()(boost::function<size_t()> f) const
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

		Matrix4f iceTransformation = Affine3f(Scaling(scale)).matrix();
		Matrix3f rotation(Quaternionf(0, cos(angle*pi / 360.f), 0, sin(angle*pi / 360.f)).toRotationMatrix());
		iceTransformation = Affine3f(rotation).matrix() * iceTransformation;
		iceTransformation = Affine3f(Translation3f(posx, 0, posz)).matrix() * iceTransformation;

		iceTransformations.push_back(iceTransformation);
	}
}



void TAndroidApplication::LoadModels()
{

	auto objData = loadObjFile("mountain/mountain.obj", "");
	mountain = ObjDataToRenderPairs(objData)[0][0];

	mountainTransformation = Affine3f(Scaling(0.5f)).matrix();
	Matrix3f rotation(Quaternionf(0, 1 * sin(pi / 8 + pi / 2), 0, 1 * cos(pi / 8 + pi / 2)).toRotationMatrix());
	mountainTransformation = Affine3f(rotation).matrix() * mountainTransformation;

	objData = loadObjFile("ice/ice.obj", "");
	ice = ObjDataToRenderPairs(objData)[0][0];
	
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
		Renderer->DrawRect(Vector2f(0.0f, 0.56f), Vector2f(1.f, 0.f), Vector2f(0.5f + SkyTexShift, 0.f), Vector2f(1.f + SkyTexShift, 1.f));

		if (TimeOfDayPref == 1)
		{
			glClear(GL_DEPTH_BUFFER_BIT);
			glBindTexture(GL_TEXTURE_2D, ResourceManager->TexList["final_cloud.png"]);
			Renderer->DrawRect(Vector2f(0.0f, 0.56f), Vector2f(4.f, 0.f), Vector2f(0.0f + SkyTexShift + CloudTimer, 0.f), Vector2f(1.f + SkyTexShift + CloudTimer, 0.5f));
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
		Renderer->DrawRect(Vector2f(0.0f, 0.54f), Vector2f(1.f, 1.f), Vector2f(0.5f + SkyTexShift, 0.f), Vector2f(1.f + SkyTexShift, 1.f));

		if (TimeOfDayPref == 1)
		{
			glClear(GL_DEPTH_BUFFER_BIT);
			glBindTexture(GL_TEXTURE_2D, ResourceManager->TexList["final_cloud.png"]);
			Renderer->DrawRect(Vector2f(0.0f, 0.54f), Vector2f(4.f, 1.f), Vector2f(0.0f + SkyTexShift + CloudTimer, 0.f), Vector2f(1.f + SkyTexShift + CloudTimer, 0.5f));
		}
	}



	Renderer->PopProjectionMatrix();

	glClear(GL_DEPTH_BUFFER_BIT);
	Renderer->SetGLCamView();

	if (inv)
	{
		Renderer->TranslateMatrix(Vector3f(0,0.1f,0));
		Renderer->ScaleMatrix(Vector3f(1, -1, 1));
	}

	Renderer->PushShader("SimplelightShader");

	Vector3f dayColor = Vector3f(0,0,0);
	Vector3f nightColor = Vector3f(0, 0.1f, 0.2f);

	if (TimeOfDayPref == 0)
	{
		RenderUniform3fv("TimeOfDayColor", dayColor.data());
		RenderUniform1f("TimeOfDayCoef1", 0.75f);
		RenderUniform1f("TimeOfDayCoef2", 0.25f);
	}
	else
		if (TimeOfDayPref == 1)
		{
			RenderUniform3fv("TimeOfDayColor", dayColor.data());
			RenderUniform1f("TimeOfDayCoef1", 0.5f);
			RenderUniform1f("TimeOfDayCoef2", 0.25f);
		}
		else
		{
			RenderUniform3fv("TimeOfDayColor", nightColor.data());
			RenderUniform1f("TimeOfDayCoef1", 0.0f);
			RenderUniform1f("TimeOfDayCoef2", 0.4f);
		}

		
		Renderer->PushPerspectiveProjectionMatrix(pi/6, Renderer->GetMatrixWidth() / Renderer->GetMatrixHeight(), 1.f, 450.f);

		{
			Renderer->PushSpecialMatrix(mountainTransformation);

			//RenderUniform4fv("light")

			TRenderParamsSetter setter(mountain.first);
			Renderer->DrawTriangleList(mountain.second);

			Renderer->PopMatrix();
		}

		{
			TRenderParamsSetter setter(ice.first);

			for (int i = 0; i < iceTransformations.size(); i++)
			{
				Renderer->PushSpecialMatrix(iceTransformations[i]);

				Renderer->DrawTriangleList(ice.second);

				Renderer->PopMatrix();
			}
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

		Renderer->DrawRect(Vector2f(0, 0), Vector2f(1.f, 1.f), Vector2f(-SkyTexShift, SnowTimer), Vector2f(multiply_x - SkyTexShift, multiply_y + SnowTimer));
		glClear(GL_DEPTH_BUFFER_BIT);
		Renderer->DrawRect(Vector2f(0, 0), Vector2f(1.f, 1.f), Vector2f(SnowTimer*0.7f+0.3f - SkyTexShift, SnowTimer+0.7f), Vector2f(multiply_x - SkyTexShift + SnowTimer*0.7f+0.3f, multiply_y + SnowTimer+0.7f));
		glClear(GL_DEPTH_BUFFER_BIT);
		Renderer->DrawRect(Vector2f(0, 0), Vector2f(1.f, 1.f), Vector2f(SnowTimer*0.4f+0.7f - SkyTexShift, SnowTimer+0.3f), Vector2f(multiply_x - SkyTexShift + SnowTimer*0.4f+0.7f, multiply_y + SnowTimer+0.3f));

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
	Renderer->ScaleMatrix(Vector3f(1, -1, 1));


	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawSceneWithoutWater(true);

	Renderer->ScaleMatrix(Vector3f(1, -1, 1));

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
	Renderer->DrawFramePartScreen("WaterFrame", Vector2f(0, 0), Vector2f(1.f, 0.54f));
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

	GetConsole() << "Inner init go!\n";

#ifdef TARGET_ANDROID
	ResourceManager->PathToResources = "";
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

	LoadingQueue.push_back(boost::function<size_t()>(boost::bind(&TTextureListClass::AddTexture, &(ResourceManager->TexList), "console_bkg.bmp", "")));
	LoadingQueue.push_back(boost::function<size_t()>(boost::bind(&TTextureListClass::AddTexture, &(ResourceManager->TexList), "water_nmap.png", "")));
	LoadingQueue.push_back(boost::function<size_t()>(boost::bind(&TTextureListClass::AddTexture, &(ResourceManager->TexList), "sky.png", "")));
	LoadingQueue.push_back(boost::function<size_t()>(boost::bind(&TTextureListClass::AddTexture, &(ResourceManager->TexList), "sky_night.png", "")));
	LoadingQueue.push_back(boost::function<size_t()>(boost::bind(&TTextureListClass::AddTexture, &(ResourceManager->TexList), "snow.png", "")));
	LoadingQueue.push_back(boost::function<size_t()>(boost::bind(&TTextureListClass::AddTexture, &(ResourceManager->TexList), "final_cloud.png", "")));
	
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

	//ResourceManager->LightManager.SetLightOn();

	//ResourceManager->LightManager.SetLightDirection(Vector3f(1, -1, 0));

	//AddFrameBuffers();

	//Renderer->SetPerspectiveFullScreenViewport();

	if (Renderer->GetScreenWidth() < Renderer->GetScreenHeight())
	{
		Renderer->SetMatrixWidth(480);
		Renderer->SetMatrixHeight(800);
	}
	
	Renderer->PushPerspectiveProjectionMatrix(pi/6, Renderer->GetMatrixWidth() / Renderer->GetMatrixHeight(), 1.f, 400.f);

	GetConsole()<<"Inner init end!\n";

	m2.unlock();
}



void TAndroidApplication::InnerDeinit()
{
	m2.lock();

	Inited = false;
	Loaded = false;

	makeShot = true;
	LoadingQueue.clear();
	
	//if (LiteModel != NULL)
	//{
	//	LiteModel->FreeModel();
	//	delete LiteModel;
	//	LiteModel = NULL;
	//}

	//IceModel.clear();

	//SimpleLand = std::shared_ptr<TSimpleLandClass>();
	//SimpleLandInv = std::shared_ptr<TSimpleLandClass>();

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
		Renderer->DrawRect(Vector2f(0, 0), Vector2f(1.f, 1.f));
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


void TAndroidApplication::InnerUpdate(size_t dt)
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

void TAndroidApplication::InnerOnMove(Vector2f shift)
{

	shift = Vector2f(shift[0]*Renderer->GetMatrixWidth()/static_cast<float>(Renderer->GetScreenWidth()), shift[1]*Renderer->GetMatrixHeight()/static_cast<float>(Renderer->GetScreenHeight()));
	
	boost::get<TPanoramicCamera>(Renderer->Camera).MoveAlpha(-pi*shift[0]*0.1f);
}

void TAndroidApplication::OnMouseDown(TMouseState& mouseState)
{
	
}