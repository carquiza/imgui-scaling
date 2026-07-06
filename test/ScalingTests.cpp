// Headless unit tests for ImGuiScaling — ScaleConfig math, the Scalable
// mixin's change-tracking state machine, and the global user-scale accessors.
// RegisterSettingsHandler needs a live ImGui context, so it is only covered
// for the no-context early-return here.

#include <gtest/gtest.h>

#include "ImGuiScaling/ImGuiScaling.hpp"

using ImGuiScaling::ScaleConfig;
using ImGuiScaling::Scalable;

// ---------------------------------------------------------------------------
// ScaleConfig
// ---------------------------------------------------------------------------

TEST(ScaleConfig, DefaultIsIdentity) {
    ScaleConfig config;
    EXPECT_FLOAT_EQ(config.dpiScale, 1.0f);
    EXPECT_FLOAT_EQ(config.userScale, 1.0f);
    EXPECT_FLOAT_EQ(config.GetEffectiveScale(), 1.0f);
}

TEST(ScaleConfig, EffectiveScaleIsProduct) {
    ScaleConfig config = ScaleConfig::Create(1.5f, 2.0f);
    EXPECT_FLOAT_EQ(config.dpiScale, 1.5f);
    EXPECT_FLOAT_EQ(config.userScale, 2.0f);
    EXPECT_FLOAT_EQ(config.GetEffectiveScale(), 3.0f);
}

TEST(ScaleConfig, FromEffectivePutsScaleOnUserComponent) {
    ScaleConfig config = ScaleConfig::FromEffective(1.75f);
    EXPECT_FLOAT_EQ(config.dpiScale, 1.0f);
    EXPECT_FLOAT_EQ(config.userScale, 1.75f);
    EXPECT_FLOAT_EQ(config.GetEffectiveScale(), 1.75f);
}

// ---------------------------------------------------------------------------
// Scalable mixin
// ---------------------------------------------------------------------------

namespace {

class TestWidget : public Scalable {
public:
    int scaleChangedCalls = 0;

    using Scalable::Scaled;
    using Scalable::ScaledTouch;

protected:
    void OnScaleChanged() override { ++scaleChangedCalls; }
};

} // namespace

TEST(Scalable, DefaultScaleIsOneWithNoPendingChange) {
    TestWidget widget;
    EXPECT_FLOAT_EQ(widget.GetScale(), 1.0f);
    EXPECT_FALSE(widget.HasScaleChanged());
}

TEST(Scalable, SetScaleUpdatesAndNotifies) {
    TestWidget widget;
    widget.SetScale(2.0f);
    EXPECT_FLOAT_EQ(widget.GetScale(), 2.0f);
    EXPECT_TRUE(widget.HasScaleChanged());
    EXPECT_EQ(widget.scaleChangedCalls, 1);
}

TEST(Scalable, AcknowledgeClearsPendingChange) {
    TestWidget widget;
    widget.SetScale(2.0f);
    widget.AcknowledgeScaleChange();
    EXPECT_FALSE(widget.HasScaleChanged());
    EXPECT_FLOAT_EQ(widget.GetScale(), 2.0f);
}

TEST(Scalable, SettingSameScaleDoesNotNotify) {
    TestWidget widget;
    widget.SetScale(2.0f);
    widget.AcknowledgeScaleChange();
    widget.SetScale(2.0f);
    EXPECT_FALSE(widget.HasScaleChanged());
    EXPECT_EQ(widget.scaleChangedCalls, 1);
}

TEST(Scalable, NonPositiveScaleIsIgnored) {
    TestWidget widget;
    widget.SetScale(0.0f);
    widget.SetScale(-1.5f);
    EXPECT_FLOAT_EQ(widget.GetScale(), 1.0f);
    EXPECT_FALSE(widget.HasScaleChanged());
    EXPECT_EQ(widget.scaleChangedCalls, 0);
}

TEST(Scalable, ScaledMultipliesByCurrentScale) {
    TestWidget widget;
    widget.SetScale(1.5f);
    EXPECT_FLOAT_EQ(widget.Scaled(10.0f), 15.0f);
}

TEST(Scalable, ScaledTouchPicksValueByMode) {
    TestWidget widget;
    widget.SetScale(2.0f);
    EXPECT_FLOAT_EQ(widget.ScaledTouch(28.0f, 48.0f, false), 56.0f);
    EXPECT_FLOAT_EQ(widget.ScaledTouch(28.0f, 48.0f, true), 96.0f);
}

// ---------------------------------------------------------------------------
// Free helpers and global user scale
// ---------------------------------------------------------------------------

TEST(ScaleHelpers, ScaleMultiplies) {
    EXPECT_FLOAT_EQ(ImGuiScaling::Scale(20.0f, 1.25f), 25.0f);
}

TEST(GlobalUserScale, RoundTripsAndRejectsNonPositive) {
    const float original = ImGuiScaling::GetUserScale();

    ImGuiScaling::SetUserScale(1.4f);
    EXPECT_FLOAT_EQ(ImGuiScaling::GetUserScale(), 1.4f);

    ImGuiScaling::SetUserScale(0.0f);
    EXPECT_FLOAT_EQ(ImGuiScaling::GetUserScale(), 1.4f);
    ImGuiScaling::SetUserScale(-2.0f);
    EXPECT_FLOAT_EQ(ImGuiScaling::GetUserScale(), 1.4f);

    ImGuiScaling::SetUserScale(original);
}

TEST(GlobalUserScale, RegisterSettingsHandlerWithoutContextIsSafe) {
    // No ImGui context exists in this test binary; the call must no-op.
    ImGuiScaling::RegisterSettingsHandler();
    SUCCEED();
}
