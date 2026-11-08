# ==============================================================================
# File: .//Description-of-file/extract_pdf_cover.md
# Overview: High-speed PDF page 1 rasterizer using pypdfium2 to generate thumbnail covers.
# ==============================================================================

import os
import sys

def extract_covers(target_dir="pdf", cache_dir=".cache"):
    try:
        import pypdfium2 as pdfium
    except ImportError:
        return False

    os.makedirs(cache_dir, exist_ok=True)
    if not os.path.exists(target_dir):
        return True

    for filename in os.listdir(target_dir):
        if not filename.lower().endswith(".pdf"):
            continue

        pdf_path = os.path.join(target_dir, filename)
        cache_path = os.path.join(cache_dir, filename + ".png")

        # Skip if thumbnail exists and is newer than the PDF file
        if os.path.exists(cache_path):
            try:
                if os.path.getmtime(cache_path) >= os.path.getmtime(pdf_path):
                    continue
            except OSError:
                pass

        try:
            doc = pdfium.PdfDocument(pdf_path)
            if len(doc) > 0:
                # Render page 1 at 2.0x scale for crisp retina-quality covers
                page = doc[0]
                image = page.render(scale=2.0).to_pil()
                image.save(cache_path, "PNG")
        except Exception:
            pass

    # Clean up obsolete cached covers for PDFs that were removed
    active_pdf_names = set(f.lower() for f in os.listdir(target_dir) if f.lower().endswith(".pdf"))
    for cached_file in os.listdir(cache_dir):
        if cached_file.lower().endswith(".png"):
            pdf_base = cached_file[:-4]
            if pdf_base.lower() not in active_pdf_names:
                try:
                    os.remove(os.path.join(cache_dir, cached_file))
                except OSError:
                    pass

    return True

if __name__ == "__main__":
    folder = sys.argv[1] if len(sys.argv) > 1 else "pdf"
    extract_covers(folder)
