SCRIPT_DIR="$(dirname "$(realpath $_)")"
VIRTUALENV_DIR="${SCRIPT_DIR}/env"

if [ ! -d "${VIRTUALENV_DIR}" ]; then
	"${SCRIPT_DIR}/setup_env.sh"
fi

# If this directory exists, we are running Windows Python inside bash
# In this kind of environment, we cannot rely on activate script because it contains
# backslashes, so we need to set up everything ourselves.
if [ -d "${VIRTUALENV_DIR}/Scripts" ]; then
	export OLD_VIRTUAL_ENV="${VIRTUAL_ENV}"
	export OLD_PATH="${PATH}"
	export OLD_PS1="${PS1}"

	export VIRTUAL_ENV="$(basename ${VIRTUALENV_DIR})"
	export PATH="${VIRTUALENV_DIR}/Scripts:${PATH}"
	export PS1="(${VIRTUAL_ENV})${PS1}"

	deactivate() {
		export VIRTUAL_ENV="${OLD_VIRTUAL_ENV}"
		export PATH="${OLD_PATH}"
		export PS1="${OLD_PS1}"

		unset OLD_VIRTUAL_ENV
		unset OLD_PATH
		unset OLD_PS1
	}
else
	source "${VIRTUALENV_DIR}/bin/activate"
fi
