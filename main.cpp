#include <TApplication.h>
#include <TBrowser.h>

#include "TUserInterface.hpp"

int main(int argc, char **argv)
{
  TApplication app("GBSFit", &argc, argv);

  auto browser = new TBrowser();

  new TUserInterface();

  app.Run();

  delete browser;

  return 0;
}
