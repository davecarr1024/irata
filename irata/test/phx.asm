ldx #$12
phx
ldx #$34
phx
ldx #$00
plx
txa
cmp #$34
jne fail
plx
txa
cmp #$12
jne fail

hlt

fail: crs
