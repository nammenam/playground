# What is this
A thicker variant of Latin Modern Roman. Details see my [blog article](https://thedrwu.com/posts/thicker-lm).

# How to build
```./adjust-lm.sh```

# Download the Font
[thick-lm-otf.zip](https://github.com/jagd/fakebold/releases/download/v1.1/thick-lm-otf.zip)
usage:

- Extract to `~/texmf/fonts/opentype/public/lm/` (create the directory if not existing)
- Install to the user/system font catalog if you want to overwrite the global latin-modern otfs

[thick-lm-pfb.zip](https://github.com/jagd/fakebold/releases/download/v1.1/thick-lm-pfb.zip)
usage:

- Extract to `~/texmf/fonts/type1/public/lm/` (create the directory if not existing)

# See also

Similar process applied to [cm-super](https://github.com/jagd/Thick-CM)

# Changelog
## v1.1
- Add lm type1 fonts
## v1.0.2
- Change stroke join to round will avoid many problems, the rounded edge is not really visible even in the zoomed document
## v1.0.1
- Does not make lowercase fraktur thicker (OTF)
