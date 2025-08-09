; test for jeq
lda #$12
cmp #$12
jeq ok
crs
ok: hlt