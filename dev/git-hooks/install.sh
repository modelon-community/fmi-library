#!/bin/sh

# This script installs a wrapper to the hook scripts to the .git's hook dir.
# By using a wrapper, we only have to call this script once instead of every
# time the file has been updated.

set -eu -o pipefail

# --- LOCAL_FUNCTIONS ---------------------------------------------- #

#  echoes all arguments to stderr
err() {
    >&2 echo "$0: ERROR: $@"
}

# --- INPUT_VALIDATION ---------------------------------------------- #
n_args_exp=0

if [[ $# -ne $n_args_exp ]]; then
    err "invalid number of input args, exp: $n_args_exp, act: $#"
    exit 1
fi

# --- SCRIPT_START -------------------------------------------------- #

src_dir="$(dirname "$(realpath "$0")")"
dst_dir="$src_dir/../../.git/hooks"

hook_dst="$dst_dir/pre-commit"
if [ -f "$hook_dst" ]; then
    echo "warning: skipping already existing hook script: $hook_dst"
else
    cat <<\EOF > "$dst_dir/pre-commit"
#!/bin/sh

# This script is just a wrapper for the hook in the repo.

dir_abs="$(dirname "$(realpath "$0")")"
. "$dir_abs/../../dev/git-hooks/pre-commit"
EOF
fi
