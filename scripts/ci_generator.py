import os
import sys
import logging
import itertools
from ruamel.yaml import YAML  # Github actions needs yaml 1.2

build_settings = ['disable_intrinsics', 'fixed_point', 'custom_modes']
platforms = ['win', 'linux', 'mac', 'ios', 'android']
archs = ['x86', 'x86_64', 'armv7', 'arm64']

# TODO: Add build support for ios
# TODO: Add support for custom scrips add hock scripts
# TODO: Add support to pick generator cmake
# TODO: Add support for gitlab
# TODO: Add support for automake
# TODO: Add build and test support for linux arm (qemu)
# TODO: Add build and test support for mingw
# TODO: Add support for cpp check
# TODO: Add support to figure out number of cores/ht on machine and use with -j
# TODO: Code coverage?


def main():
    import argparse
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('--github_action_config_path', default=os.path.abspath(os.path.join(os.path.dirname(
        __file__), '..', '.github', 'workflows')), type=str, help="github action config path (default: %(default)s)")
    parser.add_argument('--gitlab_ci_config_path', default=os.path.abspath(os.path.join(os.path.dirname(
        __file__), '..')), type=str, help="gitlab ci config path (default: %(default)s)")
    parser.add_argument('--log-level', dest='log_level', default='info', help='logging level',
                        choices=['debug', 'info', 'warning', 'error', 'critical'])

    args = parser.parse_args()

    logging.basicConfig(format='%(asctime)s %(levelname)-8s [%(pathname)s,%(lineno)d] %(message)s',
                        level=getattr(logging, args.log_level.upper()),
                        stream=sys.stdout)

    logging.info('Start')

    configs = generate_configs()

    cmake_transformer = CMakeTransformer()
    cmake_configs = cmake_transformer.transform(configs)

    github_actions_transformer = GithubActionsTransformer("CMake Build Matrix")
    github_action_configs = github_actions_transformer.transform(cmake_configs)

    github_action_yaml_file = os.path.join(
        args.github_action_config_path, 'cmake_build.yml')
    save_yaml_file(github_action_configs, github_action_yaml_file)
    logging.info("Generated github actions file {} with {} configs".format(
        github_action_yaml_file, len(cmake_configs)))

    logging.info('Stop')


def generate_configs():
    logging.info('Generate configs')
    configs = []
    combinations = list(itertools.product(
        [False, True], repeat=len(build_settings)))
    for combination in combinations:
        for platform in platforms:
            for arch in archs:
                config = {}
                config['configurations'] = dict(
                    zip(build_settings, combination))
                config['arch'] = arch
                config['platform'] = platform
                configs.append(config)
    return configs


class Transformer():
    def __init__(self):
        super(Transformer, self).__init__()

    def transform(self, item):
        assert False


class CMakeTransformer(Transformer):
    def __init__(self):
        super(CMakeTransformer, self).__init__()
        self.cmake_build = {
            'win': {'x86_64': True, 'x86': True, 'armv7': True, 'arm64': True},
            'linux': {'x86_64': True},
            'mac': {'x86_64': True},
            'android': {'x86_64': True, 'x86': True, 'armv7': True, 'arm64': True}
        }
        self.cmake_test = {
            'win': {'x86_64': True, 'x86': True},
            'linux': {'x86_64': True},
            'mac':  {'x86_64': True}
        }
        self.cmake_generator = {
            'win': '"Visual Studio 16 2019"'
        }
        self.cmake_platform_build_options = {
            'common': '-DCMAKE_BUILD_TYPE=Release',
            # https://cmake.org/cmake/help/latest/generator/Visual%20Studio%2016%202019.html
            'win': {
                'x86_64': '-A x64',
                'x86': '-A Win32',
                'armv7': '-A ARM',
                'arm64': '-A ARM64'
            },
            'android': {
                # https://developer.android.com/ndk/guides/cmake
                'common': '-DCMAKE_TOOLCHAIN_FILE=${ANDROID_HOME}/ndk-bundle/build/cmake/android.toolchain.cmake',
                'x86_64': '-DANDROID_ABI=x86_64',
                'x86': '-DANDROID_ABI=x86',
                'armv7': '-DANDROID_ABI=armeabi-v7a',
                'arm64': '-DANDROID_ABI=arm64-v8a'
            }
        }

    def transform(self, configs):
        cmake_configs = []
        for config in configs:
            if self._supported_build_target(config['platform'], config['arch']):
                d = {}
                d['configure'] = self._add_config_step(config)
                d['build'] = 'cmake --build . --config Release'
                d['name'] = self._generate_name(config)
                d['host'] = config['platform']
                if self._supported_test_target(config['platform'], config['arch']):
                    d['test'] = 'ctest'
                cmake_configs.append(d)
        return cmake_configs

    def _generate_name(self, config):
        name = config['platform'] + '-' + config['arch']
        for key, value in config['configurations'].items():
            name = name + '-' + key + '-' + ('on' if value else 'off')
        return name

    def _supported_build_target(self, platform, arch):
        try:
            return self.cmake_build[platform][arch]
        except:
            return False

    def _supported_test_target(self, platform, arch):
        try:
            return self.cmake_test[platform][arch]
        except:
            return False

    def _platform_build_option(self, platform, arch):
        platform_build_options = ''
        try:
            platform_build_options += ' -G ' + self.cmake_generator[platform]
        except:
            pass
        try:
            platform_build_options += " " + \
                self.cmake_platform_build_options['common']
        except:
            pass
        try:
            platform_build_options += " " + \
                self.cmake_platform_build_options[platform]['common']
        except:
            pass
        try:
            platform_build_options += " " + \
                self.cmake_platform_build_options[platform][arch]
        except:
            pass

        return platform_build_options

    def _add_config_step(self, config):
        configure = 'cmake .. -DOPUS_BUILD_PROGRAMS=ON -DOPUS_BUILD_TESTING=ON'
        for key, value in config['configurations'].items():
            configure += ' -DOPUS_' + key.upper() + '=' + ('ON' if value else 'OFF')
        configure += self._platform_build_option(
            config['platform'], config['arch'])
        return configure


class GithubActionsTransformer(Transformer):
    def __init__(self, name):
        super(GithubActionsTransformer, self).__init__()
        self.actions = {
            'on': [
                "push",
                "pull_request"
            ],
            "jobs": {
            },
            "name": name
        }
        self.host = {
            'win': 'windows-latest',
            'linux': 'ubuntu-latest',
            'android': 'ubuntu-latest',
            'mac': 'macos-latest',
            'ios': 'macos-latest'}
        self.workdir = 'build'

    def transform(self, configs):
        for config in configs:
            job = self._create_job(config)
            self.actions['jobs'][config['name']] = job
        return self.actions

    def _create_job(self, config):
        job = {}
        job['name'] = config['name']
        job['runs-on'] = self.host[config['host']]
        job['steps'] = [
            {
                "uses": "actions/checkout@v2",
                "with": {
                    "fetch-depth": 0
                }
            }, {
                "run": "mkdir {}".format(self.workdir),
                "name": "Create Work Dir"
            },
            {
                "working-directory": "{}".format(self.workdir),
                "run": "{}".format(config['configure']),
                "name": "Configure"
            },
            {
                "working-directory": "{}".format(self.workdir),
                "run": "{}".format(config['build']),
                "name": "Build"
            },
        ]
        if 'test' in config:
            job['steps'].append(
                {
                    "working-directory": "{}".format(self.workdir),
                    "run": "{}".format(config['test']),
                    "name": "Test"
                }
            )
        return job


def save_yaml_file(d, yaml_file):
    yaml = YAML()
    yaml.default_flow_style = False
    with open(yaml_file, 'w') as outfile:
        yaml.dump(d, outfile)


if __name__ == '__main__':
    main()
