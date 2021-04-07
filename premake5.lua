workspace "Warp"
    architecture "x64"
    startproject "Warp-Application"

    configurations { "Debug", "Release", "Dist" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "Warp-Core"
include "Warp-Application"
