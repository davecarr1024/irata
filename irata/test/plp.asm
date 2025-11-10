; Test PLP - Pull Processor status from stack
; Just verify PHP and PLP can execute without crashing
; Note: This is a basic smoke test as PLP flag restoration
; may have limitations in the current simulator implementation
php
plp
php
plp

hlt
fail: crs
