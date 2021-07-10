# Builds

This directory contains build files for all project environments.

This includes .elf and .hex files for every environment.

The naming structure for the environments is as follows:

`tinyW_XXX_Y_Z`

`W` = Divider ratio
`X` = Output pulse length
`Y` = `r` for rising edge input, `f` for falling edge input
`Z` = `i` for inverted output, `n` for non-inverted output

So:
`tiny10_30_n_r` generates a `30 ms` `positive` output pulse for every `10` `rising edges` on the input
`tiny5_250_i_f` generates a `250ms` `negative` output pulse for every `5` `falling edges` on the input
