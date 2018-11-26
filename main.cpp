#include <TApplication.h>

#include "TUserInterface.hpp"

int main(int argc, char **argv)
{
  TApplication app("GBSFit", &argc, argv);

  new TUserInterface();

  app.Run();
  return 0;
}
