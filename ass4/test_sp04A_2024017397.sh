#!/bin/sh
set -eu

here=$(CDPATH= cd "$(dirname "$0")" && pwd)
work=$(mktemp -d "${TMPDIR:-/tmp}/sp04A_test.XXXXXX")
trap 'chmod -R u+rwx "$work" 2>/dev/null; rm -rf "$work"' 0

case "$(uname -s)" in
    Darwin) get_mode() { stat -f '%Lp' "$1"; } ;;
    *)      get_mode() { stat -c '%a' "$1"; } ;;
esac

cc -std=c11 -Wall -Wextra "$here/sp04A_2024017397.c" -o "$work/sp04A"
echo 'Compile: PASS'

cd "$work"
mkdir -p tree/sub
printf 'hello\n' > tree/sub/file
printf 'outside\n' > outside
ln -s ../outside tree/link
chmod 700 tree tree/sub tree/sub/file
chmod 600 outside

echo '\n$ ./sp04A 755 tree'
./sp04A 755 tree
test "$(get_mode tree)" = 755
test "$(get_mode tree/sub)" = 755
test "$(get_mode tree/sub/file)" = 755
test "$(get_mode outside)" = 600
echo 'Recursive chmod and symlink check: PASS'

echo '\n$ ./sp04A 640 outside'
./sp04A 640 outside
test "$(get_mode outside)" = 640
echo 'Single file check: PASS'

echo '\n$ ./sp04A nope outside'
if ./sp04A nope outside > bad_mode.txt 2>&1; then
    echo 'Invalid mode check: FAIL'
    exit 1
fi
cat bad_mode.txt
echo 'Invalid mode check: PASS (nonzero exit)'

echo '\n$ ./sp04A 600 missing'
if ./sp04A 600 missing > missing.txt 2>&1; then
    echo 'Missing file check: FAIL'
    exit 1
fi
cat missing.txt
echo 'Missing file check: PASS (nonzero exit)'

echo '\nAll tests passed.'
