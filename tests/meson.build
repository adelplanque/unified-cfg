py3_exe = import('python').find_installation()

test_env = ['CFG=' + cfg.full_path()]

test('Test doc', py3_exe, args: files('run-test.py', 'test_doc.yaml'), env: test_env)
test('Test env', py3_exe, args: files('run-test.py', 'test_env.yaml'), env: test_env)
test('Test errors', py3_exe, args: files('run-test.py', 'test_errors.yaml'), env: test_env)
test('Test get', py3_exe, args: files('run-test.py', 'test_get.yaml'), env: test_env)
test('Test get2', py3_exe, args: files('run-test.py', 'test_get2.yaml'), env: test_env)
test('Test hardware', py3_exe, args: files('run-test.py', 'test_hardware.yaml'), env: test_env)
test('Test multilines', py3_exe, args: files('run-test.py', 'test_multilines.yaml'), env: test_env)
test('Test recursive', py3_exe, args: files('run-test.py', 'test_recursive.yaml'), env: test_env)
test('Test render', py3_exe, args: files('run-test.py', 'test_render.yaml'), env: test_env)
test('Test shell', py3_exe, args: files('run-test.py', 'test_shell.yaml'), env: test_env)
test('Test strings', py3_exe, args: files('run-test.py', 'test_strings.yaml'), env: test_env)
