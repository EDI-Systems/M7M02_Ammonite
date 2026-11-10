# M7M02 R3T1 English LaTeX Manual

This directory contains the English edition of the M7M02 (RVM) R3T1
Technical Reference Manual. It is a chapter-by-chapter translation of the
corrected Chinese LaTeX edition in `../Manual_CN`, which is the authoritative
R3T1 source. The old 34-page R2T1 English Word draft is not a source for this
edition.

## Build

```sh
make clean
make
```

XeLaTeX and the fonts in `../Font/` are required. The output is:

`Document/EN_M7M02_R3T1_User_Manual.pdf`

## Structure

- 9 chapters, 40 sections, 143 subsections, 183 sub-subsections and 24
  fifth-level headings.
- 99 tables, uniquely numbered per chapter, with the same column
  specifications, row counts and cell counts as the Chinese edition.
- 2 figures, redrawn in TikZ with English labels.
- 110 footnotes.
- 505 labels and 25 `\hyperref` targets, all identical to the Chinese
  edition.
- 127 pages, 0 LaTeX errors and 0 undefined references.

## Translation and audit

- Every chapter was translated in full, not summarised. Heading levels,
  paragraph order, table rows/cells, footnotes, figure captions, labels and
  citation links were preserved.
- The English text was checked against the Chinese source chapter by chapter
  by a second bilingual review pass; translation slips were corrected in the
  English files.
- All 337 code identifiers in the Chinese edition occur in the English
  edition, with no missing or extra identifier.
- The English sources contain no Chinese characters.
- Both figures are English-localised TikZ drawings: the RVM architecture
  diagram and the interrupt-priority diagram.
- The bibliography entries, which are already English-language references,
  use the same IEEE formatting and `[n]` numbering as the Chinese edition.
- The full 127-page PDF passed a page-by-page visual gate after the final
  pagination fixes: no clipped/overlapping text, broken tables, detached
  headers, orphan headings or unreadable figures.

## Source corrections

The translation follows the corrected Chinese edition, including its
documented source corrections (table 5-14 send/receive, tables 6-16/6-17,
5.6.5.5 range, 8.3 note, placeholder replacement, and the corrected 1.2.2 /
1.2.5 VMM categories and chapter-6 link). Source defects that were not
changed in the Chinese edition are mirrored in English to keep the two
editions structurally and semantically aligned. The detailed correction list
is in `../Manual_CN/differences.md`.

## Files

- `Chapter/` — nine chapters plus English front matter.
- `Figure/` — shared logo and the two English TikZ figure sources.
- `rvm-style.sty` — English-localised version of the shared visual style.
- `Makefile` — builds the root `Document/EN_M7M02_R3T1_User_Manual.pdf`.
