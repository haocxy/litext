import glob
import os
import sys
import subprocess


kUiFileSuffix = '.ui'

kOutFilePrefix = 'gen.'
kOutFileSuffix = '.h'


def should_make_cpp_file_for_ui_file(ui_file_path: str, out_file_path: str) -> bool:
	if not os.path.exists(ui_file_path):
		return False
	if not os.path.exists(out_file_path):
		return True
	ui_file_write_time = os.path.getmtime(ui_file_path)
	out_file_write_time = os.path.getmtime(out_file_path)
	return ui_file_write_time >= out_file_write_time


def make_qt_ui_cpp_file_if_needed(uic_exe_path: str, ui_file_path: str):
	if uic_exe_path is None:
		raise RuntimeError("uic_exe_path cannot be null")
	if ui_file_path is None:
		raise RuntimeError("ui_file_path cannot be null")

	ui_file_path = os.path.abspath(ui_file_path)
	ui_file_dir = os.path.dirname(ui_file_path)
	ui_file_name = os.path.basename(ui_file_path)
	out_file_path = os.path.join(ui_file_dir, kOutFilePrefix + ui_file_name + kOutFileSuffix)

	if should_make_cpp_file_for_ui_file(ui_file_path=ui_file_path, out_file_path=out_file_path):
		subprocess.run([uic_exe_path, ui_file_path, '-o', out_file_path], cwd = ui_file_dir)
		print('Qt UIC cpp file generated: %s' % out_file_path)


def is_generated_file(filepath: str) -> bool:
	filename: str = os.path.basename(filepath)
	return filename.startswith(kOutFilePrefix) and filename.endswith(kUiFileSuffix + kOutFileSuffix)


def remove_generated_file_if_needed(generated_file_path: str):
	if generated_file_path is None:
		raise RuntimeError('generated_file_path is null')
	if not is_generated_file(generated_file_path):
		return
	generated_file_dir: str = os.path.dirname(generated_file_path)
	generated_file_name: str = os.path.basename(generated_file_path)
	ui_file_name: str = generated_file_name[len(kOutFilePrefix) : -len(kOutFileSuffix)]
	ui_file_path: str = os.path.join(generated_file_dir, ui_file_name)
	if not os.path.exists(ui_file_path):
		os.remove(generated_file_path)
		print('Qt UIC cpp file removed: %s' % generated_file_path)


def update_qt_ui_cpp_files(uic_exe_path: str, source_dir: str):
	abs_source_dir = os.path.abspath(source_dir)
	for dirpath, dirnames, filenames in os.walk(abs_source_dir):
		for filename in filenames:
			filepath = os.path.join(dirpath, filename)
			if filepath.endswith(kUiFileSuffix):
				make_qt_ui_cpp_file_if_needed(uic_exe_path = uic_exe_path, ui_file_path = filepath)
			if is_generated_file(filepath):
				remove_generated_file_if_needed(generated_file_path = filepath)
	

def main():
	if len(sys.argv) < 1 + 2:
		print('Help:')
		print('\t%s <path_of_qt_uic_executable> <path_of_root_source_dir>' % os.path.basename(sys.argv[0]))
		return
	update_qt_ui_cpp_files(sys.argv[1], sys.argv[2])


if __name__ == '__main__':
	main()
