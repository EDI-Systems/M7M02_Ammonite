# M7M02 (Ammonite) R3T1 Chinese User Manual

Modern LaTeX edition of the Chinese M7M02 R3T1 technical manual.

The English edition is in `../Manual`; it is a full translation of
this corrected Chinese edition and builds `Document/EN_M7M02_R3T1_User_Manual.pdf`.

## Build

```sh
make clean
make
```

The output is `../../CN_M7M02_R3T1_User_Manual.pdf`. XeLaTeX and the shared
fonts in `../Font/` are required.

## Structure

- 9 chapters, 40 sections, 143 subsections, 183 sub-subsections and 24
  fifth-level headings. Contentless heading stubs from the original were
  removed after the audit.
- 99 numbered tables with unique per-chapter numbering (2-1 ... 8-17),
  2 figures and 110 footnotes.
- `Figure/` contains the two vector figures and the cover logo.
- `rvm-style.sty` is the shared visual language, with a dedicated display
  format for fifth-level headings.
- `differences.md` records the conversion, every corrected source defect and
  the final audit.

## Verification

- 112 pages, 196 PDF bookmarks.
- 0 LaTeX errors, 0 undefined references, no unresolved `ux...`/Pandoc
  anchors, no broken Word cross-reference fields.
- Every table has a caption and repeated headers; the Word source's merged
  groups are preserved with `\multirow` or a group subheader.
- All pages affected by the table and bibliography repair passed the final visual check.
