#include "gtest/gtest.h"

#include "eigen3/Eigen/Core"
#include "eigen3/Eigen/Geometry"

#include "msync/supported_messages/eigen_quaternion.h"
#include "msync/supported_messages/eigen_se3.h"
#include "msync/supported_policies/exact_time.h"
#include "msync/supported_policies/linear_interpolater.h"
#include "msync/supported_policies/nearest.h"
#include "msync/supported_policies/newest.h"
#include "msync/supported_storages/map_storage.h"
#include "msync/syncronizer.h"

using namespace msync;

TEST(InterfaceTest, Syncronizer) {
  using Vector3f = Eigen::Vector3f;
  using Msg = Vector3f;
  using Storage = MapStorage<Msg>;
  using Policy = ExactTimePolicy<Msg, Storage>;

  {
    using Sync = SyncronizerMinInterval<Policy, Policy>;
    Sync sync(10, Policy(10), Policy(10));
    sync.push(0, Vector3f{0, 0, 0});
    EXPECT_EQ(sync.queueSize<0>(), 1);
    EXPECT_EQ(sync.queueSize<1>(), 0);
  }

  {
    using Sync = SyncronizerMasterSlave<Policy, Policy>;
    Sync sync(Policy(10, kMaster), Policy(10));
    sync.push(0, {0, 0, 0});
  }

  {
    using Sync = HomoSyncronizerMinInterval<Policy>;
    Sync sync(10, std::vector<Policy>{Policy(10), Policy(10)});
    sync.push(0, {{0, 0, 0}, 0});
    sync.push(0, {{0, 0, 0}, 1});

    EXPECT_ANY_THROW(sync.push(0, {{0, 0, 0}, 2}));
  }

  {
    using Sync = HomoSyncronizerMasterSlave<Policy>;
    Sync sync(std::vector<Policy>{Policy(10, kMaster), Policy(10)});
    sync.push(0, {Vector3f(), 0});
    sync.push(0, {Vector3f(), 1});
    sync.push(1, {Vector3f(), 1});
    EXPECT_EQ(sync.queueSize(0), 1);
    EXPECT_EQ(sync.queueSize(1), 2);
    EXPECT_ANY_THROW(sync.queueSize(2));
  }

  {
    using P0 = ExactTimePolicy<Msg, Storage>;
    using P1 = PolicyArray<ExactTimePolicy<Msg, Storage>>;
    using Sync = SyncronizerMasterSlave<P0, P1>;
    Sync sync{P0(), {{P0(), P0()}}};
    sync.push<0>(0, Vector3f());
    sync.push<1>(0, {Vector3f(), 0});
    sync.push<1>(0, {Vector3f(), 1});
  }

  EXPECT_TRUE(true);
}

TEST(InterfaceTest, Policy) {
  {
    using Policy = ExactTimePolicy<float, MapStorage<float>>;
    using Sync = SyncronizerMinInterval<Policy, Policy>;
    Sync sync(10, Policy(10), Policy(10));
    sync.push<0>(0, float(0));
    sync.push<1>(0, float(0));
  }

  {
    using Policy = LinearInterpolatePolicy<float, MapStorage<float>>;
    using Sync = SyncronizerMinInterval<Policy, Policy>;
    Sync sync(10, Policy(10, 10), Policy(10));
    sync.push<0>(0, float(0));
    sync.push<1>(0, float(0));
  }

  {
    using Policy = NearestPolicy<float, MapStorage<float>>;
    using Sync = SyncronizerMinInterval<Policy, Policy>;
    Sync sync(10, Policy(10, 10), Policy(10));
    sync.push<0>(0, float(0));
    sync.push<1>(0, float(0));
  }

  {
    using Policy = NearestPolicy<float, MapStorage<float>>;
    using Sync = SyncronizerMinInterval<Policy, Policy>;
    Sync sync(10, Policy(10, 10), Policy(10));
    sync.push<0>(0, float(0));
    sync.push<1>(0, float(0));
  }

  {
    using Policy = NewestPolicy<float, MapStorage<float>>;
    using Sync = SyncronizerMinInterval<Policy, Policy>;
    Sync sync(10, Policy(10), Policy(10));
    sync.push<0>(0, float(0));
    sync.push<1>(0, float(0));
  }

  EXPECT_TRUE(true);
}

TEST(InterfaceTest, Storage) {
  {
    using Msg = float;
    using Storage = MapStorage<Msg>;
    using Policy = ExactTimePolicy<Msg, Storage>;
    using Sync = SyncronizerMinInterval<Policy, Policy>;
    Sync sync(10, Policy(10), Policy(10));
    sync.push<0>(0, Msg());
  }

  {
    using Msg = Eigen::Vector3d;
    using Storage =
        MapStorage<Msg, Eigen::aligned_allocator<std::pair<const Time, Msg>>>;
    using Policy = ExactTimePolicy<Msg, Storage>;
    using Sync = SyncronizerMinInterval<Policy, Policy>;
    Sync sync(10, Policy(10), Policy(10));
    sync.push<0>(0, Msg());
  }

  EXPECT_TRUE(true);
}

TEST(InterfaceTest, Message) {
  { // Vector
    using Msg = Eigen::Vector3d;
    using Storage =
        MapStorage<Msg, Eigen::aligned_allocator<std::pair<const Time, Msg>>>;
    using Policy = ExactTimePolicy<Msg, Storage>;
    using Sync = SyncronizerMinInterval<Policy, Policy>;
    Sync sync(10, Policy(10), Policy(10));
    sync.push<0>(0, Msg());
  }

  { // Quat
    using Msg = Eigen::Quaternion<double>;
    using Storage =
        MapStorage<Msg, Eigen::aligned_allocator<std::pair<const Time, Msg>>>;
    using Policy = ExactTimePolicy<Msg, Storage>;
    using Sync = SyncronizerMinInterval<Policy, Policy>;
    Sync sync(10, Policy(10), Policy(10));
    sync.push<0>(0, Msg());
  }

  { // SE3
    using Msg = Eigen::Matrix<double, 7, 1>;
    using Storage =
        MapStorage<Msg, Eigen::aligned_allocator<std::pair<const Time, Msg>>>;
    using Policy = ExactTimePolicy<Msg, Storage>;
    using Sync = SyncronizerMinInterval<Policy, Policy>;
    Sync sync(10, Policy(10), Policy(10));
    sync.push<0>(0, Msg());
  }

  EXPECT_TRUE(true);
}

TEST(InterfaceTest, QeueuSize) {
  using Msg = int;
  using Storage =
      MapStorage<Msg, Eigen::aligned_allocator<std::pair<const Time, Msg>>>;
  using Policy = ExactTimePolicy<Msg, Storage>;
  using Sync = SyncronizerMinInterval<Policy, Policy>;

  {
    Sync sync(10, Policy(10), Policy(10));

    sync.push<0>(0, 0);
    EXPECT_EQ(sync.queueSize<0>(), 1);

    sync.push<0>(2, 0);
    EXPECT_EQ(sync.queueSize<0>(), 2);

    sync.push<0>(4, 0);
    EXPECT_EQ(sync.queueSize<0>(), 3);

    sync.push<0>(6, 0);
    EXPECT_EQ(sync.queueSize<0>(), 4);

    sync.push<0>(8, 0);
    EXPECT_EQ(sync.queueSize<0>(), 5);

    sync.push<0>(10, 0);
    EXPECT_EQ(sync.queueSize<0>(), 6);

    sync.push<0>(12, 0);
    EXPECT_EQ(sync.queueSize<0>(), 6);

    sync.push<0>(14, 0);
    EXPECT_EQ(sync.queueSize<0>(), 6);
  }
}

TEST(MinIntervalPatternTest, Sanity) {
  using Msg = int;
  using Storage =
      MapStorage<Msg, Eigen::aligned_allocator<std::pair<const Time, Msg>>>;
  using Policy = ExactTimePolicy<Msg, Storage>;
  using Sync = SyncronizerMinInterval<Policy, Policy>;

  {
    Sync sync(10, Policy(100), Policy(100));

    EXPECT_EQ(sync.push<0>(0, 0), StatusCode::kMsgAccepted);
    EXPECT_EQ(sync.push<1>(0, 0), StatusCode::kMsgEmitted);
  }

  {
    Sync sync(10, Policy(100), Policy(100));

    EXPECT_EQ(sync.push<0>(0, 0), StatusCode::kMsgAccepted);
    EXPECT_EQ(sync.push<0>(0, 0), StatusCode::kMsgDropped);
  }

  {
    Sync sync(10, Policy(100), Policy(100));

    sync.push<0>(0, 0);
    sync.push<0>(5, 0);
    EXPECT_EQ(sync.push<1>(4, 0), StatusCode::kMsgAccepted);
    EXPECT_EQ(sync.timePivot(), 4);
    EXPECT_EQ(sync.push<1>(5, 0), StatusCode::kMsgEmitted);
    EXPECT_EQ(sync.timePivot(), 14);
  }

  {
    Sync sync(10, Policy(100), Policy(100));

    sync.push<0>(0, 0);
    sync.push<1>(0, 0);

    sync.push<0>(5, 0);
    sync.push<1>(5, 0);

    sync.push<0>(10, 0);
    EXPECT_EQ(sync.push<1>(10, 0), StatusCode::kMsgEmitted);
    EXPECT_EQ(sync.timePivot(), 19);
  }
}

TEST(MinIntervalPatternTest, Interpolate) {
  using Msg = float;
  using Storage =
      MapStorage<Msg, Eigen::aligned_allocator<std::pair<const Time, Msg>>>;
  using Policy = LinearInterpolatePolicy<Msg, Storage>;
  using Sync = SyncronizerMinInterval<Policy, Policy>;

  {
    Sync sync(10, Policy(100, 0), Policy(100, 0));

    EXPECT_EQ(sync.push<0>(0, 0), StatusCode::kMsgAccepted);
    EXPECT_EQ(sync.push<0>(10, 0), StatusCode::kMsgAccepted);
    EXPECT_EQ(sync.push<0>(20, 0), StatusCode::kMsgAccepted);

    EXPECT_EQ(sync.push<1>(1, 0), StatusCode::kMsgEmitted);
    EXPECT_EQ(sync.timePivot(), 10);

    EXPECT_EQ(sync.push<1>(3, 0), StatusCode::kMsgAccepted);
    EXPECT_EQ(sync.timePivot(), 10);

    EXPECT_EQ(sync.push<1>(6, 0), StatusCode::kMsgAccepted);
    EXPECT_EQ(sync.timePivot(), 10);

    EXPECT_EQ(sync.push<1>(8, 0), StatusCode::kMsgAccepted);
    EXPECT_EQ(sync.timePivot(), 10);

    EXPECT_EQ(sync.push<1>(11, 0), StatusCode::kMsgEmitted);
    EXPECT_EQ(sync.timePivot(), 20);
  }

  {
    Sync sync(10, Policy(100, 0), Policy(100, 0));

    std::vector<Time> emit_times;
    sync.registerCallback(
        [&](const Time time, const std::pair<Msg, bool> &,
            const std::pair<Msg, bool> &) { emit_times.emplace_back(time); });

    EXPECT_EQ(sync.push<0>(0, 0), StatusCode::kMsgAccepted);
    EXPECT_EQ(sync.push<0>(2, 0), StatusCode::kMsgAccepted);
    EXPECT_EQ(sync.push<0>(4, 0), StatusCode::kMsgAccepted);
    EXPECT_EQ(sync.push<0>(6, 0), StatusCode::kMsgAccepted);
    EXPECT_EQ(sync.push<0>(8, 0), StatusCode::kMsgAccepted);
    EXPECT_EQ(sync.push<0>(10, 0), StatusCode::kMsgAccepted);

    emit_times.clear();
    EXPECT_EQ(sync.push<1>(9, 0), StatusCode::kMsgEmitted);
    EXPECT_EQ(emit_times, (std::vector<Time>{9}));
  }

  {
    Sync sync(2, Policy(100, 0), Policy(100, 0));

    std::vector<Time> emit_times;
    sync.registerCallback(
        [&](const Time time, const std::pair<Msg, bool> &,
            const std::pair<Msg, bool> &) { emit_times.emplace_back(time); });

    EXPECT_EQ(sync.push<0>(0, 0), StatusCode::kMsgAccepted);
    EXPECT_EQ(sync.push<0>(10, 0), StatusCode::kMsgAccepted);

    emit_times.clear();
    EXPECT_EQ(sync.push<1>(0, 0), StatusCode::kMsgEmitted);
    EXPECT_EQ(sync.push<1>(2, 0), StatusCode::kMsgEmitted);
    EXPECT_EQ(sync.push<1>(4, 0), StatusCode::kMsgEmitted);
    EXPECT_EQ(sync.push<1>(6, 0), StatusCode::kMsgEmitted);
    EXPECT_EQ(sync.push<1>(8, 0), StatusCode::kMsgEmitted);
    EXPECT_EQ(sync.push<1>(10, 0), StatusCode::kMsgEmitted);
    EXPECT_EQ(sync.push<1>(11, 0), StatusCode::kMsgAccepted);
    EXPECT_EQ(emit_times, (std::vector<Time>{0, 2, 4, 6, 8, 10}));
  }
}
