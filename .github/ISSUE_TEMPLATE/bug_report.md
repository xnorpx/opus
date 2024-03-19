---
name: Bug Report
about: Report an issue with Opus or any of its components.
title: "[BUG] Brief description of the issue"
labels: bug
---

### Preliminary Checks

Before filing a bug, please ensure you've gone through the following:

- Checked the [Documentation](https://opus-codec.org/docs/) and [Downloads](https://opus-codec.org/downloads/) for any relevant information.
- Searched existing issues to avoid duplicates.
- For build issues, reviewed the build instructions:
  - [Autotools README](https://github.com/xiph/opus/blob/main/README)
  - [CMake README](https://github.com/xiph/opus/blob/main/cmake/README.md)
  - [Meson README](https://github.com/xiph/opus/blob/main/meson/README.md)
- For issues with Opus tools, report in the relevant repository (e.g., [libopusenc](https://github.com/xiph/libopusenc), [opusfile](https://github.com/xiph/opusfile), [opus-tools](https://github.com/xiph/opus-tools)).

### Issue Type

Select the type of issue you're reporting:
- [ ] Build Issue
- [ ] Crash
- [ ] Quality-Related Issue

### Detailed Report

#### If reporting a Build Issue:
- **Build System**: (Autotools, CMake, Meson)
- **Platform/OS**:
- **Opus Version**:
- **Build Configuration/Settings**:
- **Detailed Description & Error Logs**:

#### If reporting a Crash:
- **Operating System/Platform**:
- **Opus Version**:
- **Steps to Reproduce**:
- **Expected Behavior**:
- **Actual Behavior**:
- **Stack Trace/Dump**:

#### If reporting a Quality-Related Issue:
- **Platform/OS**:
- **Opus Version**:
- **Audio Sample Rate**:
- **Bitrate**:
- **Channel Configuration**:
- **Detailed Description & Steps to Reproduce**:
- **Attach Audio Files**:

### General

#### Steps to Reproduce (if applicable):

1. 
2. 
3. 

#### Actual vs. Expected Behavior:

- **Actual Behavior**:
- **Expected Behavior**:

#### Additional Context:

Add any other context about the problem here, such as specific settings used or the context in which the issue occurs. Please use code blocks for logs, commands, and code snippets for clarity. 