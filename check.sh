#! /bin/sh

make
python -m venv env
source env/bin/activate
#pip install pytest pyyaml
pytest tests/
