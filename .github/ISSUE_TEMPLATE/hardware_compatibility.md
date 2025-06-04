---
name: Hardware compatibility
about: Report hardware compatibility issues or request support for new hardware
title: '[HARDWARE] '
labels: hardware
assignees: ''

---

**Hardware Information**
- Component: [e.g. Different TFT display, alternate encoder, etc.]
- Part number/Model: [e.g. ILI9488, KY-040, etc.]
- Source/Manufacturer: [Where you got it]

**Current Status**
- [ ] Not working at all
- [ ] Partially working
- [ ] Working but needs configuration changes
- [ ] Requesting support for new hardware

**What's not working**
Describe the specific issues you're experiencing.

**Pin Configuration**
If using different pins than the default, list your connections:
```
ESP32-S3 Pin → Component Pin
GPIO XX → Component pin name
...
```

**Code Changes Attempted**
List any modifications you've tried:
- Pin definitions
- Library configurations  
- Timing adjustments

**Photos/Wiring**
If possible, include photos of your hardware setup (especially helpful for troubleshooting).

**Proposed Solution**
If you have ideas for how to add support or fix the issue, please share them.

**Additional Information**
- Power supply voltage/current
- Any error messages
- Behavior differences from expected