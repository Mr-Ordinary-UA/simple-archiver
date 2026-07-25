import tkinter as tk
from tkinter import filedialog, messagebox
import archive_py

rle = archive_py.RLE()

def compress_file():
    input_path = filedialog.askopenfilename(
        title="File to compress",
        filetypes=[("All files", "*.*")]
    )
    if not input_path:
        return
    output_path = filedialog.asksaveasfilename(
        title="Save archive as",
        defaultextension=".rle",
        filetypes=[("RLE archive", "*.rle"), ("All files", "*.*")]
    )
    if not output_path:
        return
    result, stats = rle.compress(input_path, output_path)
    if result == archive_py.Result.Ok:
        messagebox.showinfo("Success", f"Compressed. Ratio: {stats.ratio():.2f}")
    else:
        messagebox.showerror("Error", archive_py.result_to_string(result))

def decompress_file():
    input_path = filedialog.askopenfilename(
        title="Archive to decompress",
        filetypes=[("RLE archive", "*.rle"), ("All files", "*.*")]
    )
    if not input_path:
        return

    suggested_name = archive_py.peek_original_name(input_path)

    output_path = filedialog.asksaveasfilename(
        title="Save file as",
        initialfile=suggested_name,
        filetypes=[("All files", "*.*")]
    )
    if not output_path:
        return
    result = rle.decompress(input_path, output_path)
    if result == archive_py.Result.Ok:
        messagebox.showinfo("Success", "File restored")
    else:
        messagebox.showerror("Error", archive_py.result_to_string(result))

root = tk.Tk()
root.title("Simple Archiver")
root.geometry("300x150")

tk.Button(root, text="Compress file", command=compress_file, width=25, height=2).pack(pady=10)
tk.Button(root, text="Decompress file", command=decompress_file, width=25, height=2).pack(pady=10)

root.mainloop()