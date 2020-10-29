#include <gtest/gtest.h>

#include <core/ToyMC.hpp>

#include <core/Detector.test.hpp>
#include <core/Hit.test.hpp>
#include <core/IndexedObject.test.hpp>
#include <core/Module.test.hpp>
#include <core/Track.test.hpp>
#include <core/BranchConfig.test.hpp>
#include <core/Container.test.hpp>
#include <core/Configuration.test.hpp>
#include <core/Matching.test.hpp>
#include <core/DataHeader.test.hpp>
#include <core/Particle.test.hpp>
#include <core/EventHeader.test.hpp>

#include <infra/Field.test.hpp>
#include <infra/Variable.test.hpp>
#include <infra/SimpleCut.test.hpp>
#include <infra/Cuts.test.hpp>
#include <infra/AnalysisTask.test.hpp>
#include <infra/PlainTreeFiller.test.hpp>

int main(int argc, char **argv) {

  const int n_events = 1000;

  ToyMC<std::default_random_engine> toy_mc;
  toy_mc.GenerateEvents(n_events);
  toy_mc.WriteToFile("toy_mc.root", "fl_toy_mc.txt");

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
