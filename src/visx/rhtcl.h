struct Tcl_Interp;

extern "C" int Responsehandler_Init(Tcl_Interp* interp);
extern "C" int Eventresponsehdlr_Init(Tcl_Interp* interp);
extern "C" int Kbdresponsehdlr_Init(Tcl_Interp* interp);
extern "C" int Nullresponsehdlr_Init(Tcl_Interp* interp);
extern "C" int Serialrh_Init(Tcl_Interp* interp);
