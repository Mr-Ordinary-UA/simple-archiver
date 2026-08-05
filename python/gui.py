import tkinter as tk
from tkinter import filedialog, messagebox
import os
import archive_py

class ArchiverApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Simple Archiver")
        self.root.geometry("400x550")

        self.mode = "compress"
        self.files_to_compress = []
        self.archive_to_decompress = ""

        top_frame = tk.Frame(root)
        top_frame.pack(pady=10)

        self.btn_comp_mode = tk.Button(top_frame, text="compres", command=self.set_compress_mode, relief=tk.SUNKEN)
        self.btn_comp_mode.pack(side=tk.LEFT, padx=5)

        self.btn_decomp_mode = tk.Button(top_frame, text="decompres", command=self.set_decompress_mode)
        self.btn_decomp_mode.pack(side=tk.LEFT, padx=5)

        self.listbox = tk.Listbox(root, width=50, height=15)
        self.listbox.pack(pady=10)

        self.bottom_frame = tk.Frame(root)
        self.bottom_frame.pack(pady=10)

        self.btn_add_file = tk.Button(self.bottom_frame, text="додати файл(и)", command=self.add_file, width=25, height=2)
        self.btn_add_file.pack(pady=5)

        self.btn_add_folder = tk.Button(self.bottom_frame, text="додати папку", command=self.add_folder, width=25, height=2)
        self.btn_add_folder.pack(pady=5)

        self.btn_action = tk.Button(self.bottom_frame, text="створити архів", command=self.execute_action, width=25, height=2)
        self.btn_action.pack(pady=5)

    def set_compress_mode(self):
        self.mode = "compress"
        self.btn_comp_mode.config(relief=tk.SUNKEN)
        self.btn_decomp_mode.config(relief=tk.RAISED)
        self.btn_add_file.config(text="додати файл(и)", state=tk.NORMAL)
        self.btn_add_folder.pack(pady=5, before=self.btn_action)
        self.btn_action.config(text="створити архів")
        self.update_listbox()

    def set_decompress_mode(self):
        self.mode = "decompress"
        self.btn_comp_mode.config(relief=tk.RAISED)
        self.btn_decomp_mode.config(relief=tk.SUNKEN)
        self.btn_add_file.config(text="обрати архів", state=tk.NORMAL)
        self.btn_add_folder.pack_forget()
        self.btn_action.config(text="розпакувати")
        self.update_listbox()

    def update_listbox(self):
        self.listbox.delete(0, tk.END)
        if self.mode == "compress":
            for f in self.files_to_compress:
                self.listbox.insert(tk.END, f)
        else:
            if self.archive_to_decompress:
                self.listbox.insert(tk.END, self.archive_to_decompress)

    def add_file(self):
        if self.mode == "compress":
            file_paths = filedialog.askopenfilenames()
            if file_paths:
                self.files_to_compress.extend(file_paths)
                self.update_listbox()
        else:
            file_path = filedialog.askopenfilename()
            if file_path:
                self.archive_to_decompress = file_path
                self.update_listbox()

    def add_folder(self):
        if self.mode == "compress":
            dir_path = filedialog.askdirectory()
            if dir_path:
                self.files_to_compress.append(dir_path)
                self.update_listbox()

    def execute_action(self):
        if self.mode == "compress":
            if not self.files_to_compress:
                return
            output_path = filedialog.asksaveasfilename(defaultextension=".rle")
            if not output_path:
                return
            
            expanded_files = []
            for item in self.files_to_compress:
                if os.path.isdir(item):
                    base_dir = os.path.dirname(item)
                    rel_path = os.path.relpath(item, base_dir)
                    expanded_files.append((item, rel_path))
                    for root_dir, dirs, files in os.walk(item):
                        for d in dirs:
                            full_path = os.path.join(root_dir, d)
                            expanded_files.append((full_path, os.path.relpath(full_path, base_dir)))
                        for f in files:
                            full_path = os.path.join(root_dir, f)
                            expanded_files.append((full_path, os.path.relpath(full_path, base_dir)))
                else:
                    expanded_files.append((item, os.path.basename(item)))
            
            unique_files = []
            seen = set()
            for path, rel in expanded_files:
                if path not in seen:
                    seen.add(path)
                    unique_files.append((path, rel.replace("\\", "/")))
            
            result = archive_py.pack_files_raw(unique_files, output_path)
            if result == archive_py.Result.Ok:
                messagebox.showinfo("Success", "Архів сформовано")
                self.files_to_compress.clear()
                self.update_listbox()
            else:
                messagebox.showerror("Error", archive_py.result_to_string(result))

        else:
            if not self.archive_to_decompress:
                return
            output_dir = filedialog.askdirectory(title="Обери папку для розпакування")
            if not output_dir:
                return
            
            result = archive_py.unpack_files_raw(self.archive_to_decompress, output_dir)
            if result == archive_py.Result.Ok:
                messagebox.showinfo("Success", "Файли розпаковано")
                self.archive_to_decompress = ""
                self.update_listbox()
            else:
                messagebox.showerror("Error", archive_py.result_to_string(result))

if __name__ == "__main__":
    root = tk.Tk()
    app = ArchiverApp(root)
    root.mainloop()