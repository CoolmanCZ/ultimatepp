#include <CtrlLib/CtrlLib.h>

using namespace Upp;

#define IMAGECLASS TestImg
#define IMAGEFILE <IMLScale/test.iml>
#include <Draw/iml_header.h>

#define IMAGECLASS TestImg
#define IMAGEFILE <IMLScale/test.iml>
#include <Draw/iml_source.h>

struct MyApp : TopWindow {
	void Paint(Draw& w) override;
};

void MyApp::Paint(Draw& w)
{
	w.DrawRect(GetSize(), White());
	w.DrawImage(100, 100, TestImg::Test());
	w.DrawImage(500, 100, TestImg::Master());

	w.DrawImage(100, 200, CtrlImg::Diskette());
	DDUMP(CtrlImg::Diskette().GetSize());
}

GUI_APP_MAIN
{
	MyApp().Run();
}
