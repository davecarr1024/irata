; Test PLX - Pull X register from stack
; Push X, change it, then pull it back
ldx #$88
phx
ldx #$00
plx
txa
cmp #$88
jne fail

; Test multiple push/pull
ldx #$33
phx
ldx #$44
phx
ldx #$00
plx
txa
cmp #$44
jne fail
plx
txa
cmp #$33
jne fail

hlt
fail: crs
