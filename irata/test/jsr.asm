ldx #$03
ldy #$04
jsr add_x_and_y
cmp #$07
jne fail

hlt

fail:
crs

add_x_and_y:
txa
sta $00
tya
adc $00
rts

