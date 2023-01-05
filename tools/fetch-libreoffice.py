#!/usr/bin/python3

import requests
import sys
import tempfile
import zipfile


TRIPLET_ARCH = {
    "arm-linux-gnueabihf": "armhf",
    "aarch64-linux-gnu": "arm64",
    "x86_64-linux-gnu": "amd64",
}


class FetchLibreOfficeException(Exception):
    pass


def urlquote(s):
    return requests.utils.quote(s.encode(), safe='')


def fetch_libreoffice(project_id, ref, triplet):
    arch = TRIPLET_ARCH[triplet]

    pipeline = requests.get(
        f"https://gitlab.com/api/v4/projects/{urlquote(project_id)}/pipelines",
        params={"status": "success", "ref": ref}
    )
    pipeline.raise_for_status()
    pipeline_data = pipeline.json()
    pipeline_id = str(pipeline_data[0]["id"])
    commit = str(pipeline_data[0]["sha"])

    jobs = requests.get(
        f"https://gitlab.com/api/v4/projects/{urlquote(project_id)}/pipelines/"
        f"{urlquote(pipeline_id)}/jobs"
    )
    jobs.raise_for_status()
    job_name = f"build-libs-{arch}"
    for job in jobs.json():
        if job["name"] == job_name:
            job_id = str(job["id"])
            break
    else:
        raise FetchLibreOfficeException(f"no job for {arch} found")

    artifacts = requests.get(
        f"https://gitlab.com/api/v4/projects/{urlquote(project_id)}/jobs/"
        f"{urlquote(job_id)}/artifacts",
        stream=True
    )
    artifacts.raise_for_status()
    print(
        f"Fetched LibreOffice build of commit {commit} from project "
        f"{project_id}"
    )
    with tempfile.TemporaryFile() as tf:
        tf.write(artifacts.raw.read())
        tf.flush()
        tf.seek(0)
        with zipfile.ZipFile(tf) as zf:
            for name in zf.namelist():
                if name.startswith(f"build/{triplet}/libreoffice"):
                    print(f"Extracting {name}")
                    zf.extract(name)


if __name__ == "__main__":
    project_id = sys.argv[1]
    triplet = sys.argv[2]
    try:
        ref = sys.argv[3]
    except IndexError:
        ref = "main"
    fetch_libreoffice(project_id, ref, triplet)
