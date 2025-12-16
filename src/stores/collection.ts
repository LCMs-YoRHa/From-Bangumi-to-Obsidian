// src/stores/collection.ts
// 收藏数据状态管理

import { defineStore } from 'pinia';
import { ref, computed } from 'vue';
import { invoke } from '@tauri-apps/api/core';
import { useAuthStore } from './auth';
import type {
  CollectionListResponse,
  UserCollection,
  SubjectFullData,
  BatchImportParams,
  TaskProgress,
} from '@/types';

export const useCollectionStore = defineStore('collection', () => {
  // ============ State ============
  const collectionList = ref<CollectionListResponse | null>(null);
  const subjectIds = ref<number[]>([]);
  const currentSubject = ref<SubjectFullData | null>(null);
  const isLoading = ref<boolean>(false);
  const error = ref<string | null>(null);

  // ============ Getters ============
  const hasSubjectIds = computed(() => subjectIds.value.length > 0);
  const subjectCount = computed(() => subjectIds.value.length);

  // ============ Private Helpers ============
  function getCredentials() {
    const authStore = useAuthStore();
    if (!authStore.hasCredentials) {
      throw new Error('请先设置认证信息');
    }
    return {
      userId: authStore.userId,
      token: authStore.token,
    };
  }

  // ============ Actions ============

  /**
   * 获取用户收藏列表
   */
  async function fetchCollections(
    subjectType: number,
    collectionType?: number,
    limit: number = 30,
    offset: number = 0
  ): Promise<CollectionListResponse> {
    const { userId, token } = getCredentials();
    isLoading.value = true;
    error.value = null;

    try {
      const response = await invoke<CollectionListResponse>('get_user_collections', {
        userId,
        token,
        subjectType,
        collectionType,
        limit,
        offset,
      });
      collectionList.value = response;
      return response;
    } catch (e) {
      error.value = `获取收藏列表失败: ${e}`;
      throw e;
    } finally {
      isLoading.value = false;
    }
  }

  /**
   * 获取所有收藏的条目 ID
   */
  async function fetchAllCollectionIds(
    subjectType: number,
    collectionType?: number,
    limit: number = 30
  ): Promise<number[]> {
    const { userId, token } = getCredentials();
    isLoading.value = true;
    error.value = null;

    try {
      const ids = await invoke<number[]>('fetch_all_collection_ids', {
        userId,
        token,
        subjectType,
        collectionType,
        limit,
      });
      subjectIds.value = ids;
      return ids;
    } catch (e) {
      error.value = `获取条目 ID 列表失败: ${e}`;
      throw e;
    } finally {
      isLoading.value = false;
    }
  }

  /**
   * 获取用户单个条目收藏信息
   */
  async function fetchUserCollection(subjectId: number): Promise<UserCollection> {
    const { userId, token } = getCredentials();
    isLoading.value = true;
    error.value = null;

    try {
      const collection = await invoke<UserCollection>('get_user_collection', {
        userId,
        token,
        subjectId,
      });
      return collection;
    } catch (e) {
      error.value = `获取收藏信息失败: ${e}`;
      throw e;
    } finally {
      isLoading.value = false;
    }
  }

  /**
   * 获取条目完整信息
   */
  async function fetchSubjectFullData(subjectId: number): Promise<SubjectFullData> {
    const { userId, token } = getCredentials();
    isLoading.value = true;
    error.value = null;

    try {
      const data = await invoke<SubjectFullData>('get_subject_full_data', {
        userId,
        token,
        subjectId,
      });
      currentSubject.value = data;
      return data;
    } catch (e) {
      error.value = `获取条目信息失败: ${e}`;
      throw e;
    } finally {
      isLoading.value = false;
    }
  }

  /**
   * 更新收藏隐私设置
   */
  async function updatePrivacy(subjectId: number, isPrivate: boolean): Promise<void> {
    const { userId, token } = getCredentials();

    try {
      await invoke('update_collection_privacy', {
        userId,
        token,
        subjectId,
        isPrivate,
      });
    } catch (e) {
      throw new Error(`更新隐私设置失败: ${e}`);
    }
  }

  /**
   * 导出单个条目为 Markdown
   */
  async function exportSubject(subjectId: number, outputDir?: string): Promise<string> {
    const { userId, token } = getCredentials();

    try {
      const result = await invoke<string>('export_subject_to_markdown', {
        userId,
        token,
        subjectId,
        outputDir,
      });
      return result;
    } catch (e) {
      throw new Error(`导出条目失败: ${e}`);
    }
  }

  /**
   * 批量导出条目为 Markdown
   */
  async function batchExport(
    params: BatchImportParams,
    outputDir?: string
  ): Promise<TaskProgress> {
    const { userId, token } = getCredentials();

    try {
      const result = await invoke<TaskProgress>('batch_export_to_markdown', {
        userId,
        token,
        params,
        outputDir,
      });
      return result;
    } catch (e) {
      throw new Error(`批量导出失败: ${e}`);
    }
  }

  /**
   * 批量更新隐私设置
   */
  async function batchUpdatePrivacy(
    targetSubjectIds: number[],
    isPrivate: boolean
  ): Promise<TaskProgress> {
    const { userId, token } = getCredentials();

    try {
      const result = await invoke<TaskProgress>('batch_update_privacy', {
        userId,
        token,
        subjectIds: targetSubjectIds,
        isPrivate,
      });
      return result;
    } catch (e) {
      throw new Error(`批量更新隐私设置失败: ${e}`);
    }
  }

  /**
   * 设置条目 ID 列表
   */
  function setSubjectIds(ids: number[]) {
    subjectIds.value = ids;
  }

  /**
   * 清空状态
   */
  function reset() {
    collectionList.value = null;
    subjectIds.value = [];
    currentSubject.value = null;
    isLoading.value = false;
    error.value = null;
  }

  return {
    // State
    collectionList,
    subjectIds,
    currentSubject,
    isLoading,
    error,
    // Getters
    hasSubjectIds,
    subjectCount,
    // Actions
    fetchCollections,
    fetchAllCollectionIds,
    fetchUserCollection,
    fetchSubjectFullData,
    updatePrivacy,
    exportSubject,
    batchExport,
    batchUpdatePrivacy,
    setSubjectIds,
    reset,
  };
});
