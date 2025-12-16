// src/stores/task.ts
// 任务执行状态管理

import { defineStore } from 'pinia';
import { ref, computed } from 'vue';
import { listen } from '@tauri-apps/api/event';
import type { TaskProgress, TaskStatus } from '@/types';

export const useTaskStore = defineStore('task', () => {
  // ============ State ============
  const status = ref<TaskStatus>('idle');
  const progress = ref<TaskProgress | null>(null);
  const logs = ref<string[]>([]);
  const unlistenFunctions = ref<Array<() => void>>([]);

  // ============ Getters ============
  const isRunning = computed(() => status.value === 'running');
  const isCompleted = computed(() => status.value === 'completed');
  const isError = computed(() => status.value === 'error');
  const progressPercentage = computed(() => {
    if (!progress.value || progress.value.total === 0) return 0;
    return Math.round((progress.value.current / progress.value.total) * 100);
  });

  // ============ Actions ============

  /**
   * 开始任务
   */
  function startTask() {
    status.value = 'running';
    progress.value = null;
    logs.value = [];
  }

  /**
   * 更新进度
   */
  function updateProgress(newProgress: TaskProgress) {
    progress.value = newProgress;
    
    // 添加日志
    if (newProgress.message) {
      addLog(newProgress.message);
    }

    // 检查是否完成
    if (newProgress.is_completed) {
      status.value = 'completed';
    } else if (newProgress.is_error) {
      status.value = 'error';
    }
  }

  /**
   * 添加日志
   */
  function addLog(message: string) {
    const timestamp = new Date().toLocaleTimeString();
    logs.value.push(`[${timestamp}] ${message}`);
    
    // 限制日志数量
    if (logs.value.length > 500) {
      logs.value = logs.value.slice(-500);
    }
  }

  /**
   * 完成任务
   */
  function completeTask(message?: string) {
    status.value = 'completed';
    if (message) {
      addLog(`✅ ${message}`);
    }
  }

  /**
   * 任务出错
   */
  function failTask(errorMessage: string) {
    status.value = 'error';
    addLog(`❌ 错误: ${errorMessage}`);
  }

  /**
   * 重置任务状态
   */
  function reset() {
    status.value = 'idle';
    progress.value = null;
    logs.value = [];
  }

  /**
   * 清空日志
   */
  function clearLogs() {
    logs.value = [];
  }

  /**
   * 监听导出进度事件
   */
  async function listenExportProgress() {
    const unlisten = await listen<TaskProgress>('export-progress', (event) => {
      updateProgress(event.payload);
    });
    unlistenFunctions.value.push(unlisten);
    return unlisten;
  }

  /**
   * 监听隐私更新进度事件
   */
  async function listenPrivacyProgress() {
    const unlisten = await listen<TaskProgress>('privacy-update-progress', (event) => {
      updateProgress(event.payload);
    });
    unlistenFunctions.value.push(unlisten);
    return unlisten;
  }

  /**
   * 停止所有监听
   */
  function stopAllListeners() {
    unlistenFunctions.value.forEach((unlisten) => unlisten());
    unlistenFunctions.value = [];
  }

  return {
    // State
    status,
    progress,
    logs,
    // Getters
    isRunning,
    isCompleted,
    isError,
    progressPercentage,
    // Actions
    startTask,
    updateProgress,
    addLog,
    completeTask,
    failTask,
    reset,
    clearLogs,
    listenExportProgress,
    listenPrivacyProgress,
    stopAllListeners,
  };
});
