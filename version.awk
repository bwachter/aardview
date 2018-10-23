BEGIN {
  FS = "."
  # last allowed value before rollover.
  # i.e. 1.0.9 -> 1.1.0 for rollover=9
  rollover = 19
}
{
  if (NF == 3) {
    sum3 = $3 + 1
    sum2 = $2 + 1
    sum1 = $1 + 1
    if (int($3) < rollover) {
      printf "%s.%s.%s", $1, $2, sum3
    } else {
      if (int($2) < rollover) {
        printf "%s.%s.%s", $1, sum2, 0
      } else {
        printf "%s.%s.%s", sum1, 0, 0
      }
    }
  }
}
