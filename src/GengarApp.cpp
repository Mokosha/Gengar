#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

#include "Resources.h"

using ci::CameraPersp;
using ci::Color;
using ci::loadImage;
using ci::Sphere;
using ci::vec3;

using ci::app::App;
using ci::app::MouseEvent;
using ci::app::loadResource;
using ci::app::RendererGl;

class GengarApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
  void mouseUp( MouseEvent event ) override;
  void mouseDrag( MouseEvent event ) override;
	void update() override;
	void draw() override;

  CameraPersp		  mCamera;

  Sphere			        mEarthSphere;
  ci::gl::BatchRef	  mEarth;
  ci::gl::TextureRef	mEarthTex;

  float               mAngle;
  int                 mDownXPos;

  private:
    float GetAngleDifference( MouseEvent event ) const;
    void SetCameraAtAngle(float angle);
};

void GengarApp::setup()
{
  ci::gl::enableDepthRead();
  ci::gl::enableDepthWrite();

  mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
  mCamera.lookAt(vec3(0), vec3(0, 0, -3));

  mEarth = ci::gl::Batch::create(
    ci::geom::Plane().subdivisions(ci::vec2(10, 10)).origin(vec3(0, 0, -1))
    .size(ci::vec2(2.f*(sqrtf(2.f) - 1), 2.f*(sqrtf(2.f) - 1))).normal(vec3(0, 0, 1)),
    ci::gl::getStockShader(ci::gl::ShaderDef().texture()));
  mEarthTex = ci::gl::Texture::create(loadImage(loadResource(EARTH_TEX_RES)));

  //! This call will enable system logging, which will default to the same logging
  //! level as console/file logging.
  auto sysLogger = ci::log::makeLogger<ci::log::LoggerSystem>();

  //! This call will set the system logging level independent of the file/console logging level.
  //! The system logging level can not be lower than the file/console logging.
  sysLogger->setLoggingLevel(ci::log::LEVEL_WARNING);

  mAngle = 0;
}

void GengarApp::SetCameraAtAngle(float angle) {
  ci::quat q = ci::angleAxis(ci::toRadians(angle), vec3(0, 1, 0));
  mCamera.lookAt(vec3(0), q * vec3(0, 0, -3));
}

float GengarApp::GetAngleDifference(MouseEvent event) const {
  int diff = event.getX() - mDownXPos;

  // Get angle subtended by the difference...
  int width = ci::app::getWindowWidth();
  float fov = mCamera.getFovHorizontal();

  float degrees_per_point = fov / static_cast<float>(width);
  return static_cast<float>(diff)* degrees_per_point;
}

void GengarApp::mouseDown(MouseEvent event)
{
  mDownXPos = event.getX();
}

void GengarApp::mouseUp(MouseEvent event)
{
  float diff = GetAngleDifference(event);
  SetCameraAtAngle(mAngle + diff);
  mAngle += diff;
}

void GengarApp::mouseDrag(MouseEvent event)
{
  float diff = GetAngleDifference(event);
  SetCameraAtAngle(mAngle + diff);
}

void GengarApp::update()
{
}

void GengarApp::draw()
{
  ci::gl::clear(Color(0, 0, 0));

  ci::gl::setMatrices(mCamera);

  for (int i = 0; i < 8; ++i) {
    ci::gl::ScopedModelMatrix scpModelMtx;
    ci::gl::rotate(static_cast<float>(i)* M_PI / 4, 0.f, 1.f, 0.f);
    mEarthTex->bind();
    mEarth->draw();
  }
}

CINDER_APP(GengarApp, RendererGl, [](App::Settings *settings) {
  settings->setMultiTouchEnabled(false);
})
