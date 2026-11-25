# Basic colorigns

## Tasks
- [ ] Handling `-` lines
- [ ] Handling `x.` lines
- [ ] Handling `- [ ]`
- [ ] Handling `- [x]`
- [ ] Handling `->` & `=>`
- [ ] Handling `>>` - special color for the whole line
- [ ] Handling proper code blocks
- [ ] Hanlding `[[ ... ]]` - open close symbols

- [ ] Tokenizer
    - [x] Starting and stopping
    - [x] Differentiatie between single and double
    - [x] Detect endings
    - [x] Color sub tokens differently
    - [ ] Different start & end handling


## Descriptions
- Chees strat: just hide symbols by matching the foreground / bg color

**What do i want**
- Matching numbered lists
    -> Detecting `1234567890`
    -> Followed by `.`

**Markdown rules**
=> It either resets by repeating itself
=> Or at new line

*Except*
- Numbered list
- Unordered lists
- Checkmarks

## Thoughts
- Having double sematic tokens seems reasonable -> makes others simple
    -> Just count tokens etc
