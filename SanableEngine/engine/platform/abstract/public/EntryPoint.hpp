#pragma once

class Application;

//
//  Dynamic linkage       Static linkage              Executable
//    engine-core --> platform implementation --> game/editor/test suite
//                     SanableMain used here         Defined here
//

extern void SanableMain(Application*);
