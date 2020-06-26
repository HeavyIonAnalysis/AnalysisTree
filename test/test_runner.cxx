#include <gtest/gtest.h>

#include <core/Detector.test.hpp>
#include <core/Track.test.hpp>
#include <core/IndexedObject.test.hpp>
#include <core/Module.test.hpp>
#include <core/Hit.test.hpp>

#include <infra/Cuts.test.hpp>

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
