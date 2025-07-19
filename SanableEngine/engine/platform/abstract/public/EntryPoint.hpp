#pragma once

namespace gpr460 { class System; }

//
//  Dynamic linkage       Static linkage              Executable
//    engine-core --> platform implementation --> game/editor/test suite
//                     SanableMain used here         Defined here
//

extern void SanableMain(gpr460::System*);
