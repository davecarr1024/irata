php
pla
cmp #$00
jne fail

lda #$12
cmp #$12
php
pla
and #$02
cmp #$02
jne fail

hlt

fail: crs
