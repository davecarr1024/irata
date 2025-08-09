; Load 0x12 into a, compare it to 0x12, and fail if it isn't equal.
lda #$12
cmp #$12
jne fail
hlt
fail: crs
