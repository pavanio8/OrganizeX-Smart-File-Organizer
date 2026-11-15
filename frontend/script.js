"use strict";

class ApiClient {
  constructor(baseUrl) {
    this.baseUrl = baseUrl.replace(/\/+$/, "");
  }

  async request(path, options = {}) {
    let response;
    try {
      response = await fetch(`${this.baseUrl}${path}`, options);
    } catch (error) {
      throw new Error("Cannot reach the OrganizeX backend. Check that it is running on 127.0.0.1:8080.");
    }

    const body = await response.text();
    let data = {};
    if (body) {
      try {
        data = JSON.parse(body);
      } catch {
        throw new Error(`The backend returned an invalid response (HTTP ${response.status}).`);
      }
    }
    if (!response.ok) {
      throw new Error(data.error || `The request failed (HTTP ${response.status}).`);
    }
    return data;
  }

  status() {
    return this.request("/api/status");
  }

  preview() {
    return this.request("/api/preview");
  }

  statistics() {
    return this.request("/api/statistics");
  }

  organize() {
    return this.request("/api/organize", { method: "POST" });
  }

  upload(files, onProgress) {
    return new Promise((resolve, reject) => {
      const request = new XMLHttpRequest();
      const form = new FormData();
      for (const file of files) {
        form.append("files", file, file.name);
      }
      request.open("POST", `${this.baseUrl}/api/upload`);
      request.responseType = "json";
      request.upload.addEventListener("progress", event => {
        if (event.lengthComputable) {
          onProgress(Math.round((event.loaded / event.total) * 100));
        }
      });
      request.addEventListener("load", () => {
        const data = request.response;
        if (request.status < 200 || request.status >= 300) {
          reject(new Error(data?.error || `Upload failed (HTTP ${request.status}).`));
          return;
        }
        if (!data || typeof data !== "object") {
          reject(new Error("The backend returned an invalid upload response."));
          return;
        }
        resolve(data);
      });
      request.addEventListener("error", () => {
        reject(new Error("Cannot reach the OrganizeX backend during upload."));
      });
      request.addEventListener("timeout", () => {
        reject(new Error("The upload timed out. Check your connection and try again."));
      });
      request.timeout = 120000;
      request.send(form);
    });
  }
}

class OrganizeXApp {
  constructor(api) {
    this.api = api;
    this.selectedFiles = [];
    this.previewFiles = [];
    this.busy = false;
    this.toastTimer = null;
    this.el = {
      statusIndicator: document.getElementById("statusIndicator"),
      statusText: document.getElementById("statusText"),
      refresh: document.getElementById("refreshBtn"),
      fileInput: document.getElementById("fileInput"),
      dropArea: document.getElementById("dropArea"),
      browse: document.getElementById("browseBtn"),
      selectedFiles: document.getElementById("selectedFiles"),
      selectedCount: document.getElementById("selectedCount"),
      clearSelection: document.getElementById("clearSelectionBtn"),
      upload: document.getElementById("uploadBtn"),
      progressWrap: document.getElementById("progressWrap"),
      progressBar: document.getElementById("progressBar"),
      progressLabel: document.getElementById("progressLabel"),
      progressPercent: document.getElementById("progressPercent"),
      preview: document.getElementById("previewBtn"),
      search: document.getElementById("searchInput"),
      filter: document.getElementById("categoryFilter"),
      fileTable: document.getElementById("fileTable"),
      fileCount: document.getElementById("fileCount"),
      organize: document.getElementById("organizeBtn"),
      previewHint: document.getElementById("previewHint"),
      categoryStats: document.getElementById("categoryStats"),
      toast: document.getElementById("toast")
    };
    this.renderSelection();
    this.bindEvents();
    this.refresh();
    this.statusTimer = window.setInterval(() => this.checkStatus(), 15000);
  }

  bindEvents() {
    this.el.browse.addEventListener("click", () => this.el.fileInput.click());
    this.el.fileInput.addEventListener("change", () => {
      this.addFiles(this.el.fileInput.files);
      this.el.fileInput.value = "";
    });
    this.el.clearSelection.addEventListener("click", () => this.clearSelection());
    this.el.upload.addEventListener("click", () => this.uploadSelected());
    this.el.refresh.addEventListener("click", () => this.refresh());
    this.el.preview.addEventListener("click", () => this.refresh());
    this.el.search.addEventListener("input", () => this.renderPreview());
    this.el.filter.addEventListener("change", () => this.renderPreview());
    this.el.organize.addEventListener("click", () => this.confirmOrganize());

    this.el.dropArea.addEventListener("dragover", event => {
      event.preventDefault();
      this.el.dropArea.classList.add("dragging");
    });
    this.el.dropArea.addEventListener("dragleave", event => {
      if (!this.el.dropArea.contains(event.relatedTarget)) {
        this.el.dropArea.classList.remove("dragging");
      }
    });
    this.el.dropArea.addEventListener("drop", event => {
      event.preventDefault();
      this.el.dropArea.classList.remove("dragging");
      this.addFiles(event.dataTransfer.files);
    });
    document.querySelectorAll(".nav-link, .brand, .welcome-link").forEach(link => {
      link.addEventListener("click", () => {
        document.querySelectorAll(".nav-link").forEach(item => item.classList.remove("active"));
        document.querySelector(`.nav-link[href="${link.getAttribute("href")}"]`)?.classList.add("active");
      });
    });
    window.addEventListener("hashchange", () => this.updateActiveNavigation());
    this.updateActiveNavigation();
  }

  updateActiveNavigation() {
    const activeHash = window.location.hash || "#overview";
    document.querySelectorAll(".nav-link").forEach(link => {
      link.classList.toggle("active", link.getAttribute("href") === activeHash);
    });
  }

  addFiles(files) {
    for (const file of files) {
      this.selectedFiles.push(file);
    }
    this.renderSelection();
    if (files.length) {
      this.toast(`${files.length} file${files.length === 1 ? "" : "s"} added to the selection.`);
    }
  }

  clearSelection() {
    this.selectedFiles = [];
    this.renderSelection();
  }

  renderSelection() {
    const list = this.el.selectedFiles;
    list.replaceChildren();
    this.el.selectedCount.textContent = `${this.selectedFiles.length} file${this.selectedFiles.length === 1 ? "" : "s"}`;
    this.el.upload.disabled = this.busy;
    this.el.upload.title = this.selectedFiles.length
      ? "Upload the selected files to your local inbox."
      : "Choose files above or drag them into the upload area first.";
    this.el.clearSelection.disabled = this.busy || this.selectedFiles.length === 0;

    if (!this.selectedFiles.length) {
      const empty = document.createElement("li");
      empty.className = "selection-empty";
      empty.textContent = "Choose files to see them here.";
      list.append(empty);
      return;
    }

    this.selectedFiles.forEach((file, index) => {
      const item = document.createElement("li");
      item.className = "selected-file";
      const name = document.createElement("span");
      name.className = "selected-file-name";
      name.textContent = file.name;
      name.title = file.name;
      const size = document.createElement("span");
      size.className = "selected-file-size";
      size.textContent = this.formatSize(file.size);
      const remove = document.createElement("button");
      remove.type = "button";
      remove.className = "remove-file";
      remove.setAttribute("aria-label", `Remove ${file.name}`);
      remove.textContent = "×";
      remove.addEventListener("click", () => {
        this.selectedFiles.splice(index, 1);
        this.renderSelection();
      });
      item.append(name, size, remove);
      list.append(item);
    });
  }

  async checkStatus() {
    try {
      await this.api.status();
      this.setConnection(true, "Backend connected");
    } catch (error) {
      this.setConnection(false, "Backend disconnected");
    }
  }

  setConnection(connected, message) {
    this.el.statusIndicator.classList.toggle("online", connected);
    this.el.statusIndicator.classList.toggle("offline", !connected);
    this.el.statusText.textContent = message;
  }

  async refresh() {
    this.el.refresh.disabled = true;
    this.el.preview.disabled = true;
    try {
      const status = await this.api.status();
      this.setConnection(true, status.message || "Backend connected");
      const [preview, statistics] = await Promise.all([
        this.api.preview(),
        this.api.statistics()
      ]);
      this.previewFiles = Array.isArray(preview.files) ? preview.files : [];
      this.renderStatistics(statistics);
      this.renderPreview();
    } catch (error) {
      this.setConnection(false, "Backend disconnected");
      this.previewFiles = [];
      this.renderPreviewMessage("Backend unavailable. Start the C++ server, then refresh.");
      this.toast(error.message);
    } finally {
      this.el.refresh.disabled = this.busy;
      this.el.preview.disabled = this.busy;
    }
  }

  renderStatistics(statistics) {
    document.getElementById("totalFiles").textContent = statistics.totalFiles ?? "—";
    document.getElementById("organizedFiles").textContent = statistics.organizedFiles ?? "—";
    document.getElementById("storageUsed").textContent = this.formatSize(statistics.totalBytes);
    document.getElementById("categoriesCount").textContent = statistics.categoryCount ?? "—";
    document.getElementById("pendingSummary").textContent =
      `Pending files: ${statistics.pendingFiles ?? "—"}`;
    document.getElementById("organizedSummary").textContent =
      `Organized: ${statistics.organizedFiles ?? "—"}`;

    const categories = Object.entries(statistics.categories || {});
    const maxCount = Math.max(1, ...categories.map(([, count]) => Number(count) || 0));
    this.el.categoryStats.replaceChildren();
    this.el.filter.replaceChildren(new Option("All categories", ""));
    if (!categories.length) {
      const empty = document.createElement("p");
      empty.className = "empty-note";
      empty.textContent = "No categories are available.";
      this.el.categoryStats.append(empty);
      return;
    }
    for (const [name, count] of categories) {
      const row = document.createElement("div");
      row.className = "category-row";
      const label = document.createElement("div");
      label.className = "category-label";
      const title = document.createElement("span");
      title.textContent = name;
      const amount = document.createElement("b");
      amount.textContent = String(count);
      label.append(title, amount);
      const track = document.createElement("div");
      track.className = "category-track";
      const bar = document.createElement("span");
      bar.style.width = `${Math.max(0, Math.min(100, (Number(count) / maxCount) * 100))}%`;
      track.append(bar);
      row.append(label, track);
      this.el.categoryStats.append(row);
      this.el.filter.add(new Option(name, name));
    }
  }

  renderPreviewMessage(message) {
    this.el.fileTable.replaceChildren();
    const row = document.createElement("tr");
    const cell = document.createElement("td");
    cell.colSpan = 4;
    cell.className = "table-empty";
    cell.textContent = message;
    row.append(cell);
    this.el.fileTable.append(row);
    this.el.fileCount.textContent = "0 pending";
    this.el.organize.disabled = true;
    this.el.previewHint.textContent = "Connect to the backend to review your inbox.";
  }

  renderPreview() {
    const query = this.el.search.value.trim().toLocaleLowerCase();
    const category = this.el.filter.value;
    const files = this.previewFiles.filter(file =>
      (!category || file.category === category) &&
      (!query || file.name.toLocaleLowerCase().includes(query))
    );
    this.el.fileCount.textContent =
      `${files.length} pending file${files.length === 1 ? "" : "s"}`;
    this.el.organize.disabled = this.busy || this.previewFiles.length === 0;
    this.el.previewHint.textContent = this.previewFiles.length
      ? `${this.previewFiles.length} file${this.previewFiles.length === 1 ? "" : "s"} will be moved from Inbox when you confirm.`
      : "Your inbox is clear. Upload files to see an organization preview.";
    this.el.fileTable.replaceChildren();

    if (!files.length) {
      const row = document.createElement("tr");
      const cell = document.createElement("td");
      cell.colSpan = 4;
      cell.className = "table-empty";
      cell.textContent = this.previewFiles.length
        ? "No files match your search or category filter."
        : "Your inbox is empty. Upload files to get started.";
      row.append(cell);
      this.el.fileTable.append(row);
      return;
    }

    for (const file of files) {
      const row = document.createElement("tr");
      const name = document.createElement("td");
      name.className = "file-name-cell";
      name.textContent = file.name;
      name.title = file.name;
      const category = document.createElement("td");
      category.append(this.categoryPill(file.category));
      const size = document.createElement("td");
      size.className = "size-cell";
      size.textContent = this.formatSize(file.size);
      const destination = document.createElement("td");
      destination.className = "destination-cell";
      destination.textContent = file.destination;
      row.append(name, category, size, destination);
      this.el.fileTable.append(row);
    }
  }

  categoryPill(name) {
    const pill = document.createElement("span");
    pill.className = `category-pill category-${String(name).toLowerCase()}`;
    pill.textContent = name;
    return pill;
  }

  async uploadSelected() {
    if (this.busy) {
      return;
    }
    if (!this.selectedFiles.length) {
      this.toast("Choose files with Browse or drag them into the upload area before uploading.");
      return;
    }
    this.setBusy(true);
    this.el.upload.textContent = "Uploading…";
    this.el.progressWrap.hidden = false;
    this.updateProgress(0);
    try {
      const result = await this.api.upload(this.selectedFiles, value => this.updateProgress(value));
      this.updateProgress(100);
      const issues = Array.isArray(result.errors) ? result.errors : [];
      this.el.progressLabel.textContent = issues.length
        ? "Upload finished with errors"
        : "Upload complete";
      const completedIndexes = new Set(
        (Array.isArray(result.results) ? result.results : [])
          .filter(item => item && item.success && Number.isInteger(item.index))
          .map(item => item.index)
      );
      if (completedIndexes.size) {
        this.selectedFiles = this.selectedFiles.filter(
          (_, index) => !completedIndexes.has(index)
        );
        this.renderSelection();
      }
      if (issues.length) {
        this.toast(`${result.uploaded} uploaded; ${issues.length} failed. ${issues[0].error}`);
      } else {
        this.clearSelection();
        this.toast(`${result.uploaded} file${result.uploaded === 1 ? "" : "s"} uploaded to Inbox.`);
      }
      await this.refresh();
    } catch (error) {
      this.toast(error.message);
      await this.checkStatus();
    } finally {
      this.setBusy(false);
      this.el.upload.textContent = "Upload files ↑";
      window.setTimeout(() => {
        this.el.progressWrap.hidden = true;
      }, 1200);
    }
  }

  updateProgress(value) {
    this.el.progressBar.style.width = `${value}%`;
    this.el.progressPercent.textContent = `${value}%`;
  }

  async confirmOrganize() {
    if (!this.previewFiles.length || this.busy) {
      return;
    }
    const accepted = window.confirm(
      `Organize ${this.previewFiles.length} inbox file${this.previewFiles.length === 1 ? "" : "s"}? Files will be moved into category folders under OrganizeX_Data/Organized.`
    );
    if (!accepted) {
      return;
    }
    this.setBusy(true);
    this.el.organize.textContent = "Organizing…";
    try {
      const result = await this.api.organize();
      const failed = Array.isArray(result.results)
        ? result.results.filter(operation => !operation.success)
        : [];
      if (failed.length) {
        this.toast(`${result.moved} organized; ${failed.length} failed. ${failed[0].error}`);
      } else {
        this.toast(`${result.moved} file${result.moved === 1 ? "" : "s"} organized successfully.`);
      }
      await this.refresh();
    } catch (error) {
      this.toast(error.message);
      await this.checkStatus();
    } finally {
      this.setBusy(false);
      this.el.organize.textContent = "Organize inbox →";
    }
  }

  setBusy(busy) {
    this.busy = busy;
    this.el.upload.disabled = busy;
    this.el.clearSelection.disabled = busy || this.selectedFiles.length === 0;
    this.el.browse.disabled = busy;
    this.el.refresh.disabled = busy;
    this.el.organize.disabled = busy || this.previewFiles.length === 0;
    this.el.preview.disabled = busy;
  }

  formatSize(bytes) {
    if (!Number.isFinite(Number(bytes)) || Number(bytes) < 0) {
      return "—";
    }
    const units = ["B", "KB", "MB", "GB", "TB"];
    let size = Number(bytes);
    let unit = 0;
    while (size >= 1024 && unit < units.length - 1) {
      size /= 1024;
      unit += 1;
    }
    return `${size.toFixed(unit === 0 ? 0 : 1)} ${units[unit]}`;
  }

  toast(message) {
    this.el.toast.textContent = message;
    this.el.toast.classList.add("show");
    window.clearTimeout(this.toastTimer);
    this.toastTimer = window.setTimeout(() => this.el.toast.classList.remove("show"), 3600);
  }
}

document.addEventListener("DOMContentLoaded", () => {
  const baseUrl = document.documentElement.dataset.apiBaseUrl || "http://127.0.0.1:8080";
  new OrganizeXApp(new ApiClient(baseUrl));
});
