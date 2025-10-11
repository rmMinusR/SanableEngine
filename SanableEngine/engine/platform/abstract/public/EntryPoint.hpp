#pragma once

namespace gpr460 { class System; }

//
//  Dynamic linkage       Static linkage              Executable
//    engine-core --> platform implementation --> game/editor/test suite
//                     SanableMain used here         Defined here
//

class Application;
extern Application* SanableMain(gpr460::System*); // Really only a bootstrapping mechanism. TODO should prob rename
