# Basic colorigns

## Tasks
- [x] Hanlding `[[ ... ]]` - open close symbols
- [x] Tokenizer
    - [x] Starting and stopping
    - [x] Differentiatie between single and double
    - [x] Detect endings
    - [x] Color sub tokens differently
    - [x] Different start & end handling
    - [x] Headline & Tag patterns
- [ ] Later handlings
    - [x] Handling `-` lines
    - [ ] Handling `n.` lines
    - [x] Handling `- [ ]`
    - [x] Handling `- [x]`
    - [x] Handling `->` & `=>`
    - [ ] ~Hanlding `Properties: ` (frontmatter)~
- [ ] Code blocks
    - [x] Handling `>>` - pink
    - [x] Handling `>` - Usual
    - [ ] Code blocks - just appropriate block handling
- [ ] Extra stuff
    - [ ] End of line highlights - H1 full width
    - [ ] Special length highlights `>> etc`
    - [ ] H2 underline end of line?
    - [ ] Fixe code block proper block
    - [ ] `[[ ... | ... ]]` handling (TOK_TEXT as end token with LA?)

[[links etc]]
## Descriptions
- Chees strat: just hide symbols by matching the foreground / bg color

**What do i want**
- Matching numbered lists
    -> Detecting `1234567890`
    -> Followed by `.`
        => And then other stuff

        >> We should not do things like that

**Markdown rules**
=> It either resets by repeating itself
=> Or at new line

```bash
something else in here
# with comments etc
```

*Except*
- Numbered list
- Unordered lists
- Checkmarks

## Thoughts
- Having double sematic tokens seems reasonable -> makes others simple
    -> Just count tokens etc
