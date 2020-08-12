#include <gtest/gtest.h>

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

#include <infra/Cuts.test.hpp>

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
