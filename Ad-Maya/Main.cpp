//
// Adventure
//

#include <fstream>

#define _BOOL // for fuck's sake
#include <maya/MPxFileTranslator.h>
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>

namespace Ad
{
  class Trans :
    public MPxFileTranslator
  {
    MStatus writer (const MFileObject& dest_path, const MString& options, FileAccessMode mode) try
    {
      std::ofstream file (dest_path.fullName ().asChar ());
      if (!file)
        throw std::runtime_error ("Error opening output file");


    }
    catch (std::exception& e)
    {
      MGlobal::displayError (e.what ());
      return MS::kFailure;
    }

    bool haveWriteMethod () const
    {
      return true;
    }

    bool haveReadMethod () const
    {
      return false;
    }

    bool canBeOpened () const
    {
      return true; // I have no fucking idea
    }

    MString defaultExtension () const
    {
      return "adg";
    }

  public:
    Trans ()
    {

    }

    virtual ~Trans () = default;

  };

  void* create_trans ()
  {
    return new Trans;
  }

}

MStatus initializePlugin (MObject obj)
{
  MFnPlugin plug (obj, "Rk", "0.1");
  plug.registerFileTranslator ("Adventure", nullptr, Ad::create_trans);
  return MS::kSuccess;
}

MStatus uninitializePlugin (MObject obj)
{
  MFnPlugin plug (obj);
  plug.deregisterFileTranslator ("Adventure");
  return MS::kSuccess;
}
