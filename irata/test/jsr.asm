jsr sub
cmp #$12
jne fail

hlt

fail:
crs

sub:
lda #$12
rts
