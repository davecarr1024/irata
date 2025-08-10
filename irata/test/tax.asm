lda #$12
tax
lda #$00
txa
cmp #$12
jne fail

ldx #$34
txa
cmp #$34
jne fail

hlt

fail: crs
