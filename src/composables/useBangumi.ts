// src/composables/useBangumi.ts
// Bangumi API 调用封装

import { ref, computed } from 'vue';
import { useAuthStore } from '@/stores/auth';
import { useCollectionStore } from '@/stores/collection';
import { useTaskStore } from '@/stores/task';
import type { BatchImportParams, SubjectFullData } from '@/types';

/**
 * Bangumi API 操作 Composable
 * 
 * 封装了所有与 Bangumi API 交互的逻辑
 */
export function useBangumi() {
  const authStore = useAuthStore();
  const collectionStore = useCollectionStore();
  const taskStore = useTaskStore();

  const isLoading = ref(false);
  const error = ref<string | null>(null);

  const isAuthenticated = computed(() => authStore.isAuthenticated);

  /**
   * 获取单个条目完整信息
   */
  async function fetchSubject(subjectId: number): Promise<SubjectFullData | null> {
    if (!isAuthenticated.value) {
      error.value = '请先进行认证';
      return null;
    }

    isLoading.value = true;
    error.value = null;

    try {
      const data = await collectionStore.fetchSubjectFullData(subjectId);
      return data;
    } catch (e) {
      error.value = `获取条目失败: ${e}`;
      return null;
    } finally {
      isLoading.value = false;
    }
  }

  /**
   * 导出单个条目
   */
  async function exportSingleSubject(subjectId: number): Promise<boolean> {
    if (!isAuthenticated.value) {
      error.value = '请先进行认证';
      return false;
    }

    isLoading.value = true;
    error.value = null;
    taskStore.startTask();

    try {
      const result = await collectionStore.exportSubject(subjectId);
      taskStore.completeTask(result);
      return true;
    } catch (e) {
      error.value = `导出失败: ${e}`;
      taskStore.failTask(`${e}`);
      return false;
    } finally {
      isLoading.value = false;
    }
  }

  /**
   * 批量导出条目
   */
  async function batchExportSubjects(params: BatchImportParams): Promise<boolean> {
    if (!isAuthenticated.value) {
      error.value = '请先进行认证';
      return false;
    }

    isLoading.value = true;
    error.value = null;
    taskStore.startTask();

    try {
      // 开始监听进度事件
      await taskStore.listenExportProgress();

      const result = await collectionStore.batchExport(params);
      
      if (result.is_error) {
        taskStore.failTask(result.message);
        return false;
      }

      taskStore.completeTask(result.message);
      return true;
    } catch (e) {
      error.value = `批量导出失败: ${e}`;
      taskStore.failTask(`${e}`);
      return false;
    } finally {
      isLoading.value = false;
      taskStore.stopAllListeners();
    }
  }

  /**
   * 批量更新隐私设置
   */
  async function batchUpdatePrivacy(subjectIds: number[], isPrivate: boolean): Promise<boolean> {
    if (!isAuthenticated.value) {
      error.value = '请先进行认证';
      return false;
    }

    if (subjectIds.length === 0) {
      error.value = '没有选择任何条目';
      return false;
    }

    isLoading.value = true;
    error.value = null;
    taskStore.startTask();

    try {
      // 开始监听进度事件
      await taskStore.listenPrivacyProgress();

      const result = await collectionStore.batchUpdatePrivacy(subjectIds, isPrivate);
      
      if (result.is_error) {
        taskStore.failTask(result.message);
        return false;
      }

      taskStore.completeTask(result.message);
      return true;
    } catch (e) {
      error.value = `更新隐私设置失败: ${e}`;
      taskStore.failTask(`${e}`);
      return false;
    } finally {
      isLoading.value = false;
      taskStore.stopAllListeners();
    }
  }

  /**
   * 获取所有收藏 ID
   */
  async function fetchAllIds(
    subjectType: number,
    collectionType?: number
  ): Promise<number[]> {
    if (!isAuthenticated.value) {
      error.value = '请先进行认证';
      return [];
    }

    isLoading.value = true;
    error.value = null;

    try {
      const ids = await collectionStore.fetchAllCollectionIds(subjectType, collectionType);
      return ids;
    } catch (e) {
      error.value = `获取收藏列表失败: ${e}`;
      return [];
    } finally {
      isLoading.value = false;
    }
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
    // Computed
    isAuthenticated,
    // Methods
    fetchSubject,
    exportSingleSubject,
    batchExportSubjects,
    batchUpdatePrivacy,
    fetchAllIds,
    clearError,
  };
}
