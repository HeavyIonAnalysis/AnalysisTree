#include <gtest/gtest.h>

#include <core/BranchConfig.test.cpp>
#include <core/Configuration.test.cpp>
#include <core/Container.test.cpp>
#include <core/DataHeader.test.cpp>
#include <core/Detector.test.cpp>
#include <core/EventHeader.test.cpp>
#include <core/Hit.test.hpp>
#include <core/IndexedObject.test.cpp>
#include <core/Matching.test.cpp>
#include <core/Module.test.cpp>
#include <core/Particle.test.cpp>
#include <core/ToyMC.test.cpp>
#include <core/Track.test.cpp>

#include <infra/Cuts.test.cpp>
#include <infra/Field.test.cpp>
#include <infra/PlainTreeFiller.test.cpp>
#include <infra/SimpleCut.test.cpp>
#include <infra/VarManager.test.cpp>

int main(int argc, char **argv) {

  const int n_events = 1000;

  ToyMC<std::default_random_engine> toy_mc;
  toy_mc.GenerateEvents(n_events);
  toy_mc.WriteToFile("toy_mc.root", "fl_toy_mc.txt");

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
