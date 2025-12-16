// src/composables/useFileSystem.ts
// 文件系统操作封装

import { ref } from 'vue';
import { invoke } from '@tauri-apps/api/core';
import type { FileInfo } from '@/types';

/**
 * 文件系统操作 Composable
 */
export function useFileSystem() {
  const isLoading = ref(false);
  const error = ref<string | null>(null);
  const outputDir = ref<string>('');
  const outputFiles = ref<FileInfo[]>([]);

  /**
   * 获取应用数据目录
   */
  async function getAppDataDir(): Promise<string> {
    try {
      const dir = await invoke<string>('get_app_data_dir');
      return dir;
    } catch (e) {
      error.value = `获取应用目录失败: ${e}`;
      throw e;
    }
  }

  /**
   * 获取输出目录
   */
  async function fetchOutputDir(): Promise<string> {
    isLoading.value = true;
    error.value = null;

    try {
      const dir = await invoke<string>('get_output_dir');
      outputDir.value = dir;
      return dir;
    } catch (e) {
      error.value = `获取输出目录失败: ${e}`;
      throw e;
    } finally {
      isLoading.value = false;
    }
  }

  /**
   * 列出输出目录中的文件
   */
  async function listOutputFiles(): Promise<FileInfo[]> {
    isLoading.value = true;
    error.value = null;

    try {
      const files = await invoke<FileInfo[]>('list_output_files');
      outputFiles.value = files;
      return files;
    } catch (e) {
      error.value = `列出文件失败: ${e}`;
      throw e;
    } finally {
      isLoading.value = false;
    }
  }

  /**
   * 打开输出目录
   */
  async function openOutputDir(): Promise<void> {
    try {
      await invoke('open_output_dir');
    } catch (e) {
      error.value = `打开目录失败: ${e}`;
      throw e;
    }
  }

  /**
   * 删除输出文件
   */
  async function deleteFile(filePath: string): Promise<void> {
    try {
      await invoke('delete_output_file', { filePath });
      // 刷新文件列表
      await listOutputFiles();
    } catch (e) {
      error.value = `删除文件失败: ${e}`;
      throw e;
    }
  }

  /**
   * 检查文件是否存在
   */
  async function fileExists(filePath: string): Promise<boolean> {
    try {
      return await invoke<boolean>('file_exists', { filePath });
    } catch (e) {
      return false;
    }
  }

  /**
   * 读取文件内容
   */
  async function readFileContent(filePath: string): Promise<string> {
    try {
      return await invoke<string>('read_file_content', { filePath });
    } catch (e) {
      error.value = `读取文件失败: ${e}`;
      throw e;
    }
  }

  /**
   * 读取条目 ID 文件
   */
  async function readSubjectIds(filePath: string): Promise<number[]> {
    try {
      return await invoke<number[]>('read_subject_ids_file', { filePath });
    } catch (e) {
      error.value = `读取条目 ID 文件失败: ${e}`;
      throw e;
    }
  }

  /**
   * 保存条目 ID 到文件
   */
  async function saveSubjectIds(filePath: string, ids: number[]): Promise<void> {
    try {
      await invoke('save_subject_ids_file', { filePath, subjectIds: ids });
    } catch (e) {
      error.value = `保存条目 ID 文件失败: ${e}`;
      throw e;
    }
  }

  /**
   * 格式化文件大小
   */
  function formatFileSize(bytes: number): string {
    if (bytes < 1024) return `${bytes} B`;
    if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(1)} KB`;
    return `${(bytes / (1024 * 1024)).toFixed(1)} MB`;
  }

  /**
   * 格式化时间戳
   */
  function formatTimestamp(timestamp?: number): string {
    if (!timestamp) return '未知';
    return new Date(timestamp * 1000).toLocaleString();
  }

  /**
   * 清除错误
   */
  function clearError() {
    error.value = null;
  }

  return {
    // State
    isLoading,
    error,
    outputDir,
    outputFiles,
    // Methods
    getAppDataDir,
    fetchOutputDir,
    listOutputFiles,
    openOutputDir,
    deleteFile,
    fileExists,
    readFileContent,
    readSubjectIds,
    saveSubjectIds,
    formatFileSize,
    formatTimestamp,
    clearError,
  };
}
